#ifndef SERVER_CONFIG_HPP
#define SERVER_CONFIG_HPP

#include <string>
#include <vector>

class ServerConfig {
    private:
        std::string host;
        int port;
        std::vector<std::string> locations;

    public:
        ServerConfig();
        ServerConfig(const ServerConfig &other);
        ServerConfig &operator=(const ServerConfig &other);
        ~ServerConfig();

        std::string getHost() const;
        void setHost(const std::string &host);
        int getPort() const;
        void setPort(int port);
        std::vector<std::string> getLocations() const;
        void addLocation(const std::string &location);
};

#endif