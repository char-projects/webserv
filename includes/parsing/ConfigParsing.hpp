#ifndef CONFIG_PARSING_HPP
#define CONFIG_PARSING_HPP

#include <vector>
#include <string>

class ConfigParsing {
    private:
        std::vector<std::string *> servers;

    public:
        ConfigParsing();
        ConfigParsing(const ConfigParsing &other);
        ConfigParsing &operator=(const ConfigParsing &other);

        ~ConfigParsing();
};

#endif