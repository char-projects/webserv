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
	servers.push_back(two_server);
}

Config::~Config() {

}

// ------------------------------  (DELETEME)

Webserv::Webserv(Config configuration): active(true), configuration(configuration) {
	logger(STDOUT_FILENO, DEBUG, "Constructor Webserv called");
}

Webserv::~Webserv() {
	logger(STDOUT_FILENO, DEBUG, "Destructor Webserv called");

	for (std::map<int, ClientState>::iterator it = clients_state.begin(); it != clients_state.end(); ++it) {
		delete it->second.request;
		delete it->second.response;
	}

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
			clients_state[client_fd].response = new Response(client_fd);
			clients_state[client_fd].request = new Request(client_fd, *clients_state[client_fd].response);
			logger(STDOUT_FILENO, INFO, "New client connected in port " + stringify(client_fd));
		}
	}
}

void Webserv::clientRequest(int client_fd, bool &close_connection) {
	std::vector<char> buffer(BUFFER_SIZE);

	ssize_t bytes_read = recv(
							client_fd,
							buffer.data(),
							buffer.size(),
							0);

	if (bytes_read <= 0) {
		close_connection = true;
		// PERO OJO Con request multiples
	} else {
		clients_state[client_fd].request->setRecvData(buffer.data(), bytes_read);
		clients_state[client_fd].ready_to_write = clients_state[client_fd].request->setSendData();
	}
}

void Webserv::clientResponse(int client_fd, bool &close_connection) {
	ssize_t bytes_sent = send(
							client_fd,
							clients_state[client_fd].response->getSendData(),
							clients_state[client_fd].response->getBytesToSend(),
							0);

	if (bytes_sent <= 0) {
		close_connection = true;
	} else {
		// Si cliente respondido = its_served
		// PERO OJO Con respuestas multiples
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
				if (clients_state[client_fd].request)
					delete clients_state[client_fd].request;
				if (clients_state[client_fd].response)
					delete clients_state[client_fd].response;
				close(client_fd);
				clients_state.erase(client_fd);
				it = fds_clients.erase(it);

				logger(STDOUT_FILENO, INFO, "Client disconnected: " + stringify(client_fd));
			} else {
				++it; // ****
			}
		}
	}
}

void Webserv::stop() {
    active = false;

    for (std::map<int, ClientState>::iterator it = clients_state.begin(); it != clients_state.end(); ++it) {
        if (it->second.request) {
            delete it->second.request;
            it->second.request = NULL;
        }
        if (it->second.response) {
            delete it->second.response;
            it->second.response = NULL;
        }
    }

    for (std::vector<int>::const_iterator it = fds_clients.begin(); it != fds_clients.end(); ++it)
        close(*it);

    fds_clients.clear();
    clients_state.clear();
}
