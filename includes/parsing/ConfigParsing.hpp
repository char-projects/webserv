#ifndef CONFIG_PARSING_HPP
#define CONFIG_PARSING_HPP

#include <vector>
#include <fstream>

class ConfigParsing {
    private:
        std::vector<std::string *> servers;

    public:
        ConfigParsing();
        ConfigParsing(const ConfigParsing &other);
        ConfigParsing &operator=(const ConfigParsing &other);
        ~ConfigParsing();

        bool isFileReadable(const std::string &filePath);
};

#endif