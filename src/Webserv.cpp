#include "../includes/Webserv.hpp"

Webserv::Webserv() {}

Webserv::Webserv(const Webserv &other) {
    *this = other;
}

Webserv &Webserv::operator=(const Webserv &other) {
    (void)other;
    return *this;
}

Webserv::~Webserv() {}

Webserv::Webserv(const std::string &configFile) : configFile(configFile) {
    std::cout << "Configuration file: " << configFile << std::endl;
}

void Webserv::run() {
    ConfigParsing config;
    if (!config.isFileReadable(configFile)) {
        std::cerr << "Configuration file is not readable" << std::endl;
        return;
    }
    std::cout << "Webserver is running :)" << std::endl;
}