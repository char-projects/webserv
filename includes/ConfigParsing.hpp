#ifndef CONFIG_PARSING_HPP
#define CONFIG_PARSING_HPP

#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <sys/stat.h>
#include <cstdlib>
#include "LocationConfig.hpp"
#include "ServerConfig.hpp"

class ConfigParsing {
    private:
        std::vector<ServerConfig *> servers;

    public:
        ConfigParsing();
        ConfigParsing(const ConfigParsing &other);
        ConfigParsing &operator=(const ConfigParsing &other);
        ~ConfigParsing();

        bool isFileReadable(const std::string &filePath);
        std::vector<std::string> tokenize(const std::string &content, const std::string &delimiters);
        void parse(std::vector<std::string> &tokens);
};

#endif