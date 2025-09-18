#ifndef SERVER_CONFIG_HPP
#define SERVER_CONFIG_HPP

#include <iostream>
#include <string>
#include <vector>

class ServerConfig {
    private:
        std::string host;
        std::vector<int> ports;      
        std::vector<std::string> locations;

    public:
        ServerConfig();
        ServerConfig(const ServerConfig &other);
        ServerConfig &operator=(const ServerConfig &other);
        ~ServerConfig();

        std::string getHost() const;
        void setHost(const std::string &host);
        std::vector<int> getPorts() const;
        void setPorts(int port);
        std::vector<std::string> getLocations() const;
        void addLocation(const std::string &location);
};

#endif