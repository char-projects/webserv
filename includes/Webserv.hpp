#ifndef WEBSERV_HPP
#define WEBSERV_HPP

# include <iostream>
# include <unistd.h>
# include <string.h>

// select()
# include <sys/select.h>
# include <sys/time.h>
// sockets
# include <sys/socket.h>
# include <netinet/in.h>
# include <arpa/inet.h>

// socket non-blocking
# include <fcntl.h>
# include <iomanip>

# include <vector>
# include <map>

// Wervserv
# include "Request.hpp"
# include "Response.hpp"

# define BUFFER_SIZE 1024

// ------------------------------ Simulating Config class (DELETEME)

struct ServerConfig {
    std::vector<int> ports;
};

class Config {

	public:
		Config();
		~Config();

		std::vector<ServerConfig> servers;
};

// https://alexhost.com/faq/how-to-make-nginx-server-listen-on-multiple-ports/

// ------------------------------ (DELETEME)

class Request;
class Response;

struct ClientState {
	bool		ready_to_write;
	bool		its_served;
	Request		*request;
	Response	*response;


};

/*

clients_state[client_fd].ready_to_write = false;
clients_state[client_fd].its_served = false;
clients_state[client_fd].request = NULL;
clients_state[client_fd].response = NULL;
*/

class Webserv {

    public:
        Webserv(Config configuration);
        ~Webserv();

		void initializePorts();
        void start();
		void stop();

	private:
		Webserv();
		Webserv(const Webserv &obj);
		Webserv &operator=(const Webserv &obj);

        std::map<int, ClientState> clients_state;
		bool active;
		Config configuration;
        std::vector<int> fds_sockets;
		std::vector<int> fds_clients;
		std::vector<int> clients_served;

		int initializeSelect(fd_set &read_fds, fd_set &write_fds);
		void handleConnections(fd_set &read_fds);
		void clientRequest(int client_fd, bool &close_connection);
		void clientResponse(int client_fd, bool &close_connection);
};

#endif


/*

TODO:
- [ ] Configurar todas las conexiones con todas las opciones de la clase Config
- [ ] Controlar que todo el mensaje a llegado cuando está fragmentado
- [X] Cerrar todas las conexiones abiertas con clientes al salir
- [ ] Validar rango de puertos validos
- [ ] Limpiar estructura addr antes de configurar
- [ ] HTTP/1.1 keep-alive OJO!!!! leer cabecera request
- [ ] BUFFER_SIZE no debe ser un tamaño fijo??, leer cabecera request
- [X] Subdividir metodo start
- [ ] Hacer la parte "NULL" de excepciones del select (select(max_fd + 1, &read_fds, &write_fds, NULL, &timeout);)

TOSEE:
- [ ] NULL (bloqueo indefinido)

*/
