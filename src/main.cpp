#include "../includes/Webserv.hpp"

int main(int argc, char **argv) {
    if (argc == 2) {
        Webserv webserv(argv[1]);
        webserv.run();
    } else {
        std::cerr << "Usage: " << argv[0] << " <config_file>" << std::endl;
        return 1;
    }
}