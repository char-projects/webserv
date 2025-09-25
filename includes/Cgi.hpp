#ifndef CGI_HPP
#define CGI_HPP

#include <iostream>
#include <map>
#include <cstring>
#include <sys/wait.h> // waitpid

class Cgi {
    private:
        std::string scriptPath;
        std::map<std::string, std::string> env;
        std::string output;
        int status;

    public:
        Cgi(const std::string &scriptPath, const std::map<std::string, std::string> &env);
        Cgi(const Cgi &other);
        Cgi &operator=(const Cgi &other);
        ~Cgi();

        bool execute();
        std::string getOutput() const;
        int getStatus() const;
};

#endif