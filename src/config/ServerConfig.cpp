#include "../../includes/config/ServerConfig.hpp"

ServerConfig::ServerConfig() : host(""), port(0) {}

ServerConfig::ServerConfig(const ServerConfig &other) {
    *this = other;
}

ServerConfig &ServerConfig::operator=(const ServerConfig &other) {
    if (this != &other) {
        this->host = other.host;
        this->port = other.port;
        this->locations = other.locations;
    }
    return *this;
}

ServerConfig::~ServerConfig() {}

std::string ServerConfig::getHost() const {
    return host;
}

int ServerConfig::getPort() const {
    return port;
}

std::vector<std::string> ServerConfig::getLocations() const {
    return locations;
}

void ServerConfig::setHost(const std::string &host) {
    this->host = host;
}

void ServerConfig::setPort(int port) {
    this->port = port;
}

void ServerConfig::addLocation(const std::string &location) {
    locations.push_back(location);
}
