#include "../includes/Webserv.hpp"
// #include "../includes/Config.hpp"

int main(int argc, char **argv) {
 
    std::string config_file = "configs/default.conf";
    // Webserv *webserv = NULL;

    if (argc > 1)
        config_file = argv[1];

    Webserv webserv(config_file);
    ConfigParsing config;
    config.setConfigFile(config_file);
    if (!config.isFileReadable(config_file)) {
        std::cerr << "Configuration file is not readable" << std::endl;
        return 1;
    }
    std::vector<std::string> tokens = config.tokenize(config_file, " \n\t");
    config.parse(tokens);
    std::cout << "Webserver is running :)" << std::endl;

    //webserv = new Webserv(config);
    
    /* webserv = new Webserv();
    webserv->start();

    delete webserv; */
}