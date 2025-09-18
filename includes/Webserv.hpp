#ifndef WEBSERV_HPP
#define WEBSERV_HPP

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include <sys/select.h>
#include <sys/time.h>
#include <vector>

#include "ConfigParsing.hpp"
#include "LocationConfig.hpp"

class Webserv {

    public:
        //Webserv(const Config& config);
        Webserv();
        Webserv(const Webserv &other);
        Webserv &operator=(const Webserv &other);
        ~Webserv();
        void start();

        Webserv(const std::string &configFile);
        void run();

    private:
        //std::map<int, Client> clients;
        std::vector<int> fds_socket;
};

#endif