
#ifndef UTILS_H
# define UTILS_H


# include <iostream>
# include <string>
# include <fstream>
# include <unistd.h>
# include <sstream>

# define LOG_FILE	3
# define LOG_NAME	"/home/csubires/.cd42/42_webserv/src/logger.log"

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

#endif
