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

struct ClientState {
	std::string request;
	std::string response;
	bool ready_to_write;
};


class Webserv {

    public:
        Webserv(const Config& configuration);
        ~Webserv();
		void initialize();
        void start();
		void stop();

	private:
        Webserv();
		bool active;
		const Config& configuration;
        std::vector<int> fds_sockets;
		std::vector<int> fds_clients;
        std::map<int, ClientState> clients_state;
};

#endif


/*

TODO:
- [ ] Configurar todas las conexiones con todas las opciones de la clase Config
- [ ] Controlar que todo el mensaje a llegado cuando está fragmentado
- [ ] Cerrar todas las conexiones abiertas con clientes al salir
- [ ] Validar rango de puertos validos
- [ ] Limpiar estructura addr antes de configurar
- [ ] HTTP/1.1 keep-alive OJO!!!! leer cabecera request
- [ ] BUFFER_SIZE no debe ser un tamaño fijo??, leer cabecera request
- [ ] Subdividir metodo start


*/
