#ifndef UTILS_H
# define UTILS_H

# include <iostream>
# include <string>
# include <fstream>
# include <unistd.h>
# include <sstream>
# include <vector>

// pathIsFile
#include <sys/stat.h>

# define DEBUG_MODE	true
# define LOG_FILE	3
# define LOG_NAME	"./logger.log"

enum e_message {
    ERROR,
    WARNING,
    INFO,
    DEBUG,
    SUCCESS
};

template<typename T>
std::string stringify(const T& value) {
    std::stringstream ss;
    ss << value;
    return ss.str();
}

void logger(int out, e_message type, const std::string& message);
std::string format_string(const std::string& format, const std::vector<std::string>& args);
bool pathIsFile(const std::string& path);

#endif
