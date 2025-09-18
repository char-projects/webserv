#include "../../includes/config/ServerConfig.hpp"

ServerConfig::ServerConfig() : host(""), ports() {}

ServerConfig::ServerConfig(const ServerConfig &other) {
    *this = other;
}

ServerConfig &ServerConfig::operator=(const ServerConfig &other) {
    if (this != &other) {
        this->host = other.host;
        this->ports = other.ports;
        this->locations = other.locations;
    }
    return *this;
}

ServerConfig::~ServerConfig() {}

std::string ServerConfig::getHost() const {
    return host;
}

std::vector<int> ServerConfig::getPorts() const {
    return ports;
}

std::vector<std::string> ServerConfig::getLocations() const {
    return locations;
}

void ServerConfig::setHost(const std::string &host) {
    this->host = host;
    std::cout << "Server name: " << this->host << std::endl;
}

void ServerConfig::setPorts(int port) {
    this->ports.push_back(port);
    std::cout << "Added port: " << port << std::endl;
}

void ServerConfig::addLocation(const std::string &location) {
    locations.push_back(location);
}
