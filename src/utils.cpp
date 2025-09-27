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
				std::ofstream file(DEFAULT_ERROR_LOG, std::ios::app);
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

std::string format_string(const std::string& format, const std::vector<std::string>& args) {
	std::string			result = format;
	std::stringstream	ss;

	for (size_t i = 0; i < args.size(); ++i) {
		size_t pos = result.find("{}");
		if (pos != std::string::npos)
			result.replace(pos, 2, args[i]);
	}

	return (result);
}

std::string trim(const std::string& str) {
	size_t first = str.find_first_not_of(" \t\n\r");
	if (first == std::string::npos) {
		return ("");
	}
	size_t last = str.find_last_not_of(" \t\n\r");
	return (str.substr(first, last - first + 1));
}

PathType checkPath(const std::string& path) {
	struct stat s;

	if (stat(path.c_str(), &s)) {
		struct stat s2;
		if (!lstat(path.c_str(), &s2)) {
			return (PATH_NO_PERMISSION);
		} else {
			if (errno == ENOENT)
				return (PATH_NOT_EXISTS);
			else if (errno == EACCES)
				return (PATH_NO_PERMISSION);
			else
				return (PATH_ERROR);
		}
	}

	if (S_ISREG(s.st_mode))
		return (PATH_IS_FILE);
	else if (S_ISDIR(s.st_mode))
		return (PATH_IS_DIRECTORY);
	else
		return (PATH_IS_OTHER);
}

bool pathIsFile(const std::string& path) {
	return (checkPath(path) == (PATH_IS_FILE));
}


bool pathIsDirectory(const std::string& path) {
	return (checkPath(path) == (PATH_IS_DIRECTORY));
}

bool pathExists(const std::string& path) {
	PathType result = checkPath(path);
	return (result == PATH_IS_FILE ||
			result == PATH_IS_DIRECTORY ||
			result == PATH_IS_OTHER ||
			result == PATH_NO_PERMISSION);
}

bool pathIsAccessible(const std::string& path) {
	PathType result = checkPath(path);
	return (result == PATH_IS_FILE ||
			result == PATH_IS_DIRECTORY ||
			result == PATH_IS_OTHER);
}
