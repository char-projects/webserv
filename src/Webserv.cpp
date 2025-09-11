#include "../includes/Webserv.hpp"

Webserv::Webserv() {
    // TODO inicializar estructura cliente + config

} 

Webserv::~Webserv() {
    // TODO Cerrar todos los descriptores de clientes

    for (int socket: fds_socket)
    {
        close(socket);
    }

}

void Webserv::start() {

    std::cout << "Starting up server" << std::endl;

    sockaddr_in server_addr;
    int new_socket;
    int opt = 1;
    int addrlen = sizeof(server_addr);
    char buffer[100];

    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        std::cout << "Error creating socket" << std::endl;
        exit(1);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(9999);

    if (bind(fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        std::cout << "Error bind socket" << std::endl;
        close(fd);
    }

    if (listen(fd, 10) < 0) {
        std::cout << "Error listen socket" << std::endl;
        exit(1);
    }

    new_socket = accept(fd, (struct sockaddr *)&server_addr, (socklen_t *)&addrlen);
    if (new_socket < 0) {
        std::cout << "Error accept socket" << std::endl;
        exit(1);
    }

    // TODO REQUEST
    auto bytesRead = read(new_socket, buffer, 100);
    std::cout << "Dimeee: " << buffer;

    // TODO RESPONSE
    std::string response = "HolAAAAAA\n";
    send(new_socket, response.c_str(), response.size(), 0);

    close(new_socket);
    close(fd);

    // PAra multiserver
    /*
        fds_socket.push_back(fd);
    */

}
