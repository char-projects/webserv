#include "../includes/Webserv.hpp"
#include "../includes/utils.hpp"

// ------------------------------ Simulation Config class (DELETEME)

Config::Config() {
	ServerConfig one_server;
	one_server.ports.push_back(8080);
	one_server.ports.push_back(8081);
	one_server.ports.push_back(8082);
	servers.push_back(one_server);

	ServerConfig two_server;
	two_server.ports.push_back(8083);
	two_server.ports.push_back(8084);
	two_server.ports.push_back(8085);
    servers.push_back(two_server);
}

Config::~Config() {

}

// ------------------------------  (DELETEME)

Webserv::Webserv(const Config& configuration): active(true), configuration(configuration) {
	logger(STDOUT_FILENO, DEBUG, "Constructor Webserv called");
}

Webserv::Webserv(const Webserv &obj):
	active(obj.active),
    configuration(obj.configuration),
    fds_sockets(obj.fds_sockets),
	fds_clients(obj.fds_clients),
    clients_state(obj.clients_state)
{
	logger(STDOUT_FILENO, DEBUG, "Webserv object constructor called");
}

Webserv	&Webserv::operator=(const Webserv &obj)
{
	logger(STDOUT_FILENO, DEBUG, "Webserv assignation operator called");
	if (this != &obj)
		this->active = obj.active;
	return (*this);
}

Webserv::~Webserv() {
	logger(STDOUT_FILENO, DEBUG, "Destructor Webserv called");

	for (std::vector<int>::const_iterator it = fds_clients.begin(); it != fds_clients.end(); ++it)
		close(*it);

	for (std::vector<int>::const_iterator it = fds_sockets.begin(); it != fds_sockets.end(); ++it)
		close(*it);

}

void Webserv::initializePorts() {
	logger(STDOUT_FILENO, INFO, "Initializing listening ports");

	int fd_socket;
    int opt = 1;
    int flags;

	const std::vector<ServerConfig> servers = configuration.servers;
	for (std::vector<ServerConfig>::const_iterator it = servers.begin(); it != servers.end(); ++it) {

		const std::vector<int> ports = (*it).ports;
		for (std::vector<int>::const_iterator it2 = ports.begin(); it2 != ports.end(); ++it2) {

			sockaddr_in server_addr;
			server_addr.sin_family = AF_INET;
			server_addr.sin_addr.s_addr = INADDR_ANY;
			server_addr.sin_port = htons(*it2);

			if ((fd_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
				throw std::runtime_error("The port " + stringify(ntohs(server_addr.sin_port)) + " could not be created.");

			if (setsockopt(fd_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
    			throw std::runtime_error("Error setting socket parameters.");

			if (bind(fd_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0)
				throw std::runtime_error("Port assignment error " + stringify(ntohs(server_addr.sin_port)));

			if (listen(fd_socket, 10) < 0)
				throw std::runtime_error("Error listening on port " + stringify(ntohs(server_addr.sin_port)));

			flags = fcntl(fd_socket, F_GETFL, 0);
			if (flags == -1 || fcntl(fd_socket, F_SETFL, flags | O_NONBLOCK) == -1)
				throw std::runtime_error("Error setting socket non-blocking " + stringify(ntohs(server_addr.sin_port)));

			fds_sockets.push_back(fd_socket);
			logger(STDOUT_FILENO, INFO, "Listening at the port " + stringify(*it2));
		}
	}
}

int Webserv::initializeSelect(fd_set &read_fds, fd_set &write_fds) {
    
    int max_fd = 0;
    int client_fd;
    struct timeval timeout = {5, 0}; // NULL (bloqueo indefinido)
    FD_ZERO(&read_fds);
    FD_ZERO(&write_fds);

    for (size_t i = 0; i < fds_sockets.size(); ++i) {
        FD_SET(fds_sockets[i], &read_fds);
        if (fds_sockets[i] > max_fd)
            max_fd = fds_sockets[i];
    }

    for (size_t i = 0; i < fds_clients.size(); ++i) {
        client_fd = fds_clients[i];
        FD_SET(client_fd, &read_fds);

        if (clients_state[client_fd].ready_to_write)
            FD_SET(client_fd, &write_fds);

        if (client_fd > max_fd)
            max_fd = client_fd;
    }

    return (select(max_fd + 1, &read_fds, &write_fds, NULL, &timeout));
}

void Webserv::handleConnections(fd_set &read_fds) {

    int client_fd;

    for (size_t i = 0; i < fds_sockets.size(); ++i) {

        if (FD_ISSET(fds_sockets[i], &read_fds)) {
            struct sockaddr_in client_addr;
            socklen_t client_len = sizeof(client_addr);
            client_fd = accept(fds_sockets[i], (struct sockaddr*)&client_addr, &client_len);

            if (client_fd < 0) {
                logger(STDOUT_FILENO, ERROR, "Error establishing incoming connection.");
                continue;
            }

            fds_clients.push_back(client_fd);
            clients_state[client_fd] = ClientState();
            logger(STDOUT_FILENO, INFO, "New client connected in port " + stringify(client_fd));
        }
    }
}

void Webserv::clientRequest(int client_fd, bool &close_connection) {
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

void Webserv::clientResponse(int client_fd, bool &close_connection) {
    ssize_t bytes_sent = send(client_fd, clients_state[client_fd].response.c_str(), clients_state[client_fd].response.size(), 0);

    if (bytes_sent <= 0) {
        close_connection = true;
    } else {
        std::string log_msg = "Sent (" + stringify(bytes_sent) + " bytes) to client " + stringify(client_fd);
        logger(STDOUT_FILENO, SUCCESS, log_msg);
        close_connection = true;
    }
}

void Webserv::start() {
    logger(STDOUT_FILENO, INFO, "Server ready to establish connections with clients");

    int activity;
    int client_fd;
    bool close_connection;

    fd_set read_fds, write_fds;

    while (active) {

        activity = initializeSelect(read_fds, write_fds);

        if (activity < 0) {
            if (errno != EINTR)
				throw std::runtime_error("Error in select() ");
            continue;
        }

        if (!activity) {
            logger(STDOUT_FILENO, DEBUG, "Timeout in select() - no activity");
            continue;
        }

        handleConnections(read_fds);

        for (std::vector<int>::iterator it = fds_clients.begin(); it != fds_clients.end(); ) {

			client_fd = *it;
            close_connection = false;

            if (FD_ISSET(client_fd, &read_fds))
                clientRequest(client_fd, close_connection);

            if (!close_connection && FD_ISSET(client_fd, &write_fds) && clients_state[client_fd].ready_to_write)
                clientResponse(client_fd, close_connection);

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
