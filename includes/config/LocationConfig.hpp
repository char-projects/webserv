#ifndef LOCATION_CONFIG_HPP
#define LOCATION_CONFIG_HPP

#include <string>
#include <vector>

class LocationConfig {
    private:
        std::string path;
        std::vector<std::string> allowedMethods;

    public:
        LocationConfig();
        LocationConfig(const LocationConfig &other);
        LocationConfig &operator=(const LocationConfig &other);
        ~LocationConfig();

        std::string getPath() const;
        void setPath(const std::string &path);
        std::vector<std::string> getAllowedMethods() const;
        void addAllowedMethod(const std::string &method);
};

#endif