#ifndef WEBSERV_HPP
#define WEBSERV_HPP

#include <iostream>
#include "parsing/ConfigParsing.hpp"

class Webserv {
    private:
        std::string configFile;
        
    public:
        Webserv();
        Webserv(const Webserv &other);
        Webserv &operator=(const Webserv &other);
        ~Webserv();

        Webserv(const std::string &configFile);
        void run();
};

#endif