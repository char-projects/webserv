#include <csignal>
#include <cstdlib>

#include "../includes/Webserv.hpp"
#include "../includes/utils.hpp"
// #include "../includes/Config.hpp"


Webserv *webserv = NULL;

void signalHandler(int signum) {
	logger(STDOUT_FILENO, DEBUG, "Interrupt signal received");

    if (webserv) {
        webserv->stop();
    }
}

int main(int argc, char **argv, char **envp) {

    std::string config_file = "config/default.conf";

    if (argc > 1)
        config_file = argv[1];

	Config configuration;
	// configuration.parse(config_file) ......
	//webserv = new Webserv(config);

	signal(SIGINT, signalHandler);

	try {
		webserv = new Webserv(configuration);
		webserv->initialize();
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
