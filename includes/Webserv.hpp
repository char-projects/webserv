#ifndef WEBSERV_HPP
# define WEBSERV_HPP

# include <iostream>
# include <vector>
# include <map>
# include <unistd.h>
# include <string.h>
// select
# include <sys/select.h>
# include <sys/time.h>
// sockets
# include <sys/socket.h>
# include <netinet/in.h>
# include <arpa/inet.h>
// socket non-blocking
# include <fcntl.h>
# include <iomanip>
// Wervserv
# include "Request.hpp"
# include "Response.hpp"
# include "ResponseHeader.hpp"
# include "ConfigParsing.hpp"
# include "ServerConfig.hpp"

# define SERVER_NAME		"Webserv/42.0"
# define BUFFER_RECV_SIZE	8192
# define MAX_BODY_SIZE		1024
# define UPLOADS			"www/upload"


class Request;
class Response;
class ConfigParsing;

struct ClientState {
	bool			ready_to_read;
	bool			ready_to_write;
	bool			its_served;
	bool			receiving_body;
	ServerConfig	*config;
	Request			*request;
	Response		*response;
	time_t			last_activity;
	size_t			expected_body_size;
	size_t			total_bytes_received;
};

class Webserv {

	public:
		Webserv(ConfigParsing& config);
		~Webserv();

		void initializePorts();
		void start();
		void stop();

	private:
		ConfigParsing&					config;
		std::map<int, ClientState>		clients_state;
		bool							active;
		std::map<int, ServerConfig*>	fds_sockets;
		std::vector<int>				fds_clients;
		std::vector<int>				clients_served;
		static Webserv*					signal_instance;

		Webserv();
		Webserv(const Webserv &obj);
		Webserv &operator=(const Webserv &obj);

		int		initializeSelect(fd_set &read_fds, fd_set &write_fds);
		void	handleConnections(fd_set &read_fds);
		void	clientRequest(int client_fd, bool &close_connection);
		void	clientResponse(int client_fd, bool &close_connection);

		static void setSignalInstance(Webserv* instance) {
			signal_instance = instance;
		}

		static void signalHandler(int signum) {
			(void)signum;
			if (signal_instance)
				signal_instance->stop();
		}
};

#endif
