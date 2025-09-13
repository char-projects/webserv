#include "../includes/Webserv.hpp"
#include "../includes/utils.hpp"

// ------------------------------ Simulation Config class (DELETEME)

Config::Config() {
	ServerConfig one_server;
	one_server.ports.push_back(8080);
	one_server.ports.push_back(8081);
	one_server.ports.push_back(8082);
	servers.push_back(one_server);
}

Config::~Config() {

}

// ------------------------------  (DELETEME)


Webserv::Webserv(const Config& configuration): active(true), configuration(configuration) {

	logger(STDOUT_FILENO, DEBUG, "Constructor Webserv called");
}

Webserv::~Webserv() {

	logger(STDOUT_FILENO, DEBUG, "Destructor Webserv called");

	for (std::vector<int>::const_iterator it = fds_clients.begin(); it != fds_clients.end(); ++it)
		close(*it);

	for (std::vector<int>::const_iterator it = fds_sockets.begin(); it != fds_sockets.end(); ++it)
		close(*it);

}

void Webserv::initialize() {

	logger(STDOUT_FILENO, INFO, "Initializing listening ports");
	int fd_socket;

	const std::vector<ServerConfig> servers = configuration.servers;
	for (std::vector<ServerConfig>::const_iterator it = servers.begin(); it != servers.end(); ++it) {

		const std::vector<int> ports = (*it).ports;
		for (std::vector<int>::const_iterator it2 = ports.begin(); it2 != ports.end(); ++it2) {

			sockaddr_in server_addr;
			server_addr.sin_family = AF_INET;
			server_addr.sin_addr.s_addr = INADDR_ANY;
			server_addr.sin_port = htons(*it2);

			if ((fd_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
				throw std::runtime_error("Error creating socket " + stringify(ntohs(server_addr.sin_port)));

			int opt = 1;
			if (setsockopt(fd_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
    			throw std::runtime_error("Error setting SO_REUSEADDR");

			if (bind(fd_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0)
				throw std::runtime_error("Error bind socket " + stringify(ntohs(server_addr.sin_port)));

			if (listen(fd_socket, 10) < 0)
				throw std::runtime_error("Error listen socket " + stringify(ntohs(server_addr.sin_port)));

			int flags = fcntl(fd_socket, F_GETFL, 0);
			if (flags == -1 || fcntl(fd_socket, F_SETFL, flags | O_NONBLOCK) == -1)
				throw std::runtime_error("Error setting socket non-blocking");

			fds_sockets.push_back(fd_socket);
			logger(STDOUT_FILENO, INFO, "Listening at the port " + stringify(*it2));
		}
	}
}

void Webserv::start() {
    logger(STDOUT_FILENO, INFO, "Server ready to establish connections with clients");

    while (active) {

        fd_set read_fds, write_fds;
        FD_ZERO(&read_fds);
        FD_ZERO(&write_fds);

        int max_fd = 0;

        for (size_t i = 0; i < fds_sockets.size(); ++i) {
            FD_SET(fds_sockets[i], &read_fds);
            if (fds_sockets[i] > max_fd)
				max_fd = fds_sockets[i];
        }

        for (size_t i = 0; i < fds_clients.size(); ++i) {
            int client_fd = fds_clients[i];
            FD_SET(client_fd, &read_fds);

            if (clients_state[client_fd].ready_to_write)
                FD_SET(client_fd, &write_fds);

            if (client_fd > max_fd)
				max_fd = client_fd;
        }

        struct timeval timeout = {5, 0}; // NULL (bloqueo indefinido)
        int activity = select(max_fd + 1, &read_fds, &write_fds, NULL, &timeout);

        if (activity < 0) {
            if (errno != EINTR)
				throw std::runtime_error("Error in select() ");
            continue;
        }

        if (activity == 0) {
            logger(STDOUT_FILENO, DEBUG, "Timeout in select() - no activity");
            continue;
        }

        for (size_t i = 0; i < fds_sockets.size(); ++i) {

			if (FD_ISSET(fds_sockets[i], &read_fds)) {
				struct sockaddr_in client_addr;
                socklen_t client_len = sizeof(client_addr);
                int client_fd = accept(fds_sockets[i], (struct sockaddr*)&client_addr, &client_len);

                if (client_fd < 0) {
                    logger(STDOUT_FILENO, ERROR, "Error accepting connection");
                    continue;
                }

                fds_clients.push_back(client_fd);
                clients_state[client_fd] = ClientState();
                logger(STDOUT_FILENO, INFO, "New client connected: " + stringify(client_fd));
            }
        }

        for (std::vector<int>::iterator it = fds_clients.begin(); it != fds_clients.end(); ) {

			int client_fd = *it;
            bool close_connection = false;

            if (FD_ISSET(client_fd, &read_fds)) {
                char buffer[BUFFER_SIZE] = {0};
                ssize_t bytes_read = recv(client_fd, buffer, sizeof(buffer), 0);

                if (bytes_read <= 0) {
                    close_connection = true;
                } else {
                    clients_state[client_fd].request.append(buffer, bytes_read);

                    std::string request_log = "Received (" + stringify(bytes_read) + " bytes) from client " + stringify(client_fd);
                    logger(STDOUT_FILENO, SUCCESS, request_log);
					logger(STDOUT_FILENO, SUCCESS, buffer);

                    std::string content = "<h1>Probando probando!!!!</h1>";
                    clients_state[client_fd].response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: " + stringify(content.length()) + "\r\n\r\n" + content;
                    clients_state[client_fd].ready_to_write = true;
                }
            }

            if (!close_connection && FD_ISSET(client_fd, &write_fds) && clients_state[client_fd].ready_to_write) {
                ssize_t bytes_sent = send(client_fd, clients_state[client_fd].response.c_str(), clients_state[client_fd].response.size(), 0);

                if (bytes_sent <= 0) {
                    close_connection = true;
                } else {
                    std::string log_msg = "Sent (" + stringify(bytes_sent) + " bytes) to client " + stringify(client_fd);
                    logger(STDOUT_FILENO, SUCCESS, log_msg);
                    close_connection = true;
                }
            }

            if (close_connection) {
                close(client_fd);
                clients_state.erase(client_fd);
                it = fds_clients.erase(it);
                logger(STDOUT_FILENO, INFO, "Client disconnected: " + stringify(client_fd));
            } else {
                ++it;
            }
        }
    }
}

void Webserv::stop() {
	active = false;
	// delete this;

	for (std::vector<int>::const_iterator it = fds_clients.begin(); it != fds_clients.end(); ++it)
		close(*it);

}
