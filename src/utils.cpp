#include "../includes/utils.hpp"

void logger(int out, e_message type, const std::string& message) {
    const char* color_reset = "\033[0m";
    const char* color = color_reset;
 	const char* prefix = "";

	switch (type) {
        case ERROR:
			color = "\033[1;31m";
			prefix = "[ERROR] ";
            break;
        case WARNING:
			color = "\033[1;33m";
			prefix = "[WARNING] ";
            break;
        case INFO:
            color = "\033[1;34m";
			prefix = "[INFO] ";
            break;
        case DEBUG:
            color = "\033[1;35m";
			prefix = "[DEBUG] ";
            break;
        case SUCCESS:
            color = "\033[1;32m";
			prefix = "[SUCCESS] ";
            break;
        default:
            color = color_reset;
			prefix = "[UNKNOWN] ";
    }

    switch (out) {
        case STDOUT_FILENO:
            std::cout << color << prefix << message << color_reset << std::endl << std::flush;
            break;
        case STDERR_FILENO:
            std::cerr << prefix << message << color_reset << std::endl << std::flush;
            break;
        case LOG_FILE:
            {
                std::ofstream file(LOG_NAME, std::ios::app);
                if (file.is_open()) {
                    file << prefix << message << std::endl << std::flush;
                    file.close();
                } else {
                    std::cerr << "[ERROR] Logger " << std::endl << std::flush;
                }
            }
            break;
        default:
            std::cerr << "[ERROR] Unknown out: " << out << std::endl << std::flush;
            break;
    }
}
