#include "../includes/Webserv.hpp"
// #include "../includes/Config.hpp"

int main(int argc, char **argv, char **envp) {
 
    std::string config_file = "config/default.conf";
    Webserv *webserv = NULL;

    if (argc > 1)
        config_file = argv[1];

    //Config config .read/parse .....;

    //webserv = new Webserv(config);
    webserv = new Webserv();
    webserv->start();

    delete webserv;

    return (0);

}