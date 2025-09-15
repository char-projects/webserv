#include <csignal>
#include <cstdlib>

#include "../includes/Webserv.hpp"
#include "../includes/utils.hpp"
// #include "../includes/Config.hpp"


Webserv *webserv = NULL;

void signalHandler(int signum) {
	(void) signum;
	logger(STDOUT_FILENO, DEBUG, "Interrupt signal received");

    if (webserv)
        webserv->stop();
}

int main(int argc, char **argv, char **envp) {
	(void) envp;

	std::string config_file = "config/default.conf";

	signal(SIGINT, signalHandler);
	
	if (argc > 1)
		config_file = argv[1];

		// configuration.parse(config_file) ......
		//webserv = new Webserv(config);
		
	Config configuration;

	try {
		webserv = new Webserv(configuration);
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
