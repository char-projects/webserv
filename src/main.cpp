#include "../includes/Webserv.hpp"
// #include "../includes/Config.hpp"

int main(int argc, char **argv) {
 
    std::string config_file = "configs/default.conf";
    // Webserv *webserv = NULL;

    if (argc > 1)
        config_file = argv[1];

    Webserv webserv(config_file);
    webserv.run();

    //webserv = new Webserv(config);
    
    /* webserv = new Webserv();
    webserv->start();

    delete webserv; */
}