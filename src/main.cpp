#include <csignal>
#include <cstdlib>

#include "../includes/Webserv.hpp"
#include "../includes/ConfigParsing.hpp"
#include "../includes/utils.hpp"

Webserv *webserv = NULL;

void signalHandler(int signum) {
	(void) signum;
	logger(STDOUT_FILENO, DEBUG, "Interrupt signal received");

    if (webserv)
        webserv->stop();
}

int main(int argc, char **argv) {
 
    std::string config_file = "configs/default.conf";
;

    if (argc > 1)
        config_file = argv[1];

    ConfigParsing config;
    config.setConfigFile(config_file);
    if (!config.isFileReadable(config_file)) {
        std::cerr << "Configuration file is not readable" << std::endl;
        return 1;
    }
    std::vector<std::string> tokens = config.tokenize(config_file, " \n\t");
    config.parse(tokens);
    std::cout << "Webserver is running :)" << std::endl;

    signal(SIGINT, signalHandler);
	
	if (argc > 1)
		config_file = argv[1];

	try {
		webserv = new Webserv(config);
		webserv->initializePorts();
		webserv->start();

	} catch (const std::exception &e) {
		logger(STDOUT_FILENO, ERROR, e.what());

        if (webserv) {
            delete webserv;
            webserv = NULL;
        }

		return (EXIT_FAILURE);
	}

    return (EXIT_SUCCESS);
}