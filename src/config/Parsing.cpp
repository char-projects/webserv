#include "../../includes/parsing/ConfigParsing.hpp"

ConfigParsing::ConfigParsing() {}

ConfigParsing::ConfigParsing(const ConfigParsing &other) {
    *this = other;
}

ConfigParsing &ConfigParsing::operator=(const ConfigParsing &other) {
    if (this != &other) {
        this->servers = other.servers;
    }
    return *this;
}

ConfigParsing::~ConfigParsing() {
    for (size_t i = 0; i < servers.size(); i++)
        delete servers[i];
    servers.clear();
}

bool ConfigParsing::isFileReadable(const std::string &filePath) {
    std::ifstream file(filePath.c_str());
    return file.good();
}
