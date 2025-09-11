#include "../../includes/config/LocationConfig.hpp"

LocationConfig::LocationConfig() {}

LocationConfig::LocationConfig(const LocationConfig &other) {
    *this = other;
}

LocationConfig &LocationConfig::operator=(const LocationConfig &other) {
    if (this != &other) {
        this->path = other.path;
        this->allowedMethods = other.allowedMethods;
    }
    return *this;
}

LocationConfig::~LocationConfig() {}

std::string LocationConfig::getPath() const {
    return path;
}

void LocationConfig::setPath(const std::string &path) {
    this->path = path;
}

std::vector<std::string> LocationConfig::getAllowedMethods() const {
    return allowedMethods;
}

void LocationConfig::addAllowedMethod(const std::string &method) {
    allowedMethods.push_back(method);
}
