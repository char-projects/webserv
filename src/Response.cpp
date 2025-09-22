#include "../includes/Response.hpp"
#include "../includes/utils.hpp"

Response::Response(const int client_fd, const Request& request, const ServerConfig& config)
	: client_fd(client_fd), request(request), config(config) {
	logger(STDOUT_FILENO, DEBUG, "Constructor Response called");
	send_response.clear();
	send_header.clear();
	send_body.clear();
	status_code = 200;
	bytes_send = 0;
	counter = 0;
	response_header = new ResponseHeader();
}

Response::~Response() {
	logger(STDOUT_FILENO, DEBUG, "Destructor Response called");
	delete response_header;
}

const char* Response::getResponse() {
	status_code = request.status_code;
	logger(STDOUT_FILENO, DEBUG, "Processing request - Method: " + request.method + ", Path: " + request.path + ", Status: " + stringify(status_code));

	if (status_code == 200) {
		if (request.method == "GET") {
			readContent(request.path);
		} else if (request.method == "POST") {
			status_code = 501; // TODOOOOO
			readContent(getPathStatusCode());
		} else if (request.method == "DELETE") {
			deleteContent(request.path);
		} else {
			status_code = 405;
			readContent(getPathStatusCode());
		}
	} else {
		readContent(getPathStatusCode());
	}

	logger(STDOUT_FILENO, DEBUG, "Body size before headers: " + stringify(send_body.size()));
	response_header->setContentLength(send_body.size());
	response_header->setContentType(request.path);
	response_header->setLocation(request.location);
	send_header = response_header->getHeader(status_code);
	send_response.clear();
	send_response.append(send_header);
	send_response.append(send_body);
	logger(STDOUT_FILENO, DEBUG, "Final response size: " + stringify(send_response.size()));
	logger(STDOUT_FILENO, DEBUG, "Headers:\n" + send_header);

	return (send_response.data());
}

size_t Response::getSize() {
	return (send_response.size());
}

const std::string Response::getPathStatusCode() {




	/*
	std::vector<std::string> pairs = config.getLocations();
	for (std::vector<std::pair<int, std::string> >::const_iterator it = pairs.begin(); it != pairs.end(); ++it) {
		for (std::vector<std::pair<int, std::string> >::const_iterator it = pairs.begin(); it != pairs.end(); ++it) {
			std::cout << "Processing: " << it->first << " - " << it->second << std::endl;
		}
	}
	*/

	// TEMPORAL DELETEME

	switch (status_code) {
		case 200: return "";
		case 201: return "";
		case 204: return "";
		case 301: return "error_pages/301.html";
		case 400: return "error_pages/400.html";
		case 403: return "error_pages/403.html";
		case 404: return "error_pages/404.html";
		case 405: return "error_pages/405.html";
		case 413: return "error_pages/413.html";
		case 500: return "error_pages/500.html";
		default: return "error_pages/500.html";
	}

	// TEMPORAL DELETEME
}

void Response::readContent(const std::string &path) {
	std::ifstream file;
	std::stringstream buffer;

	if (++counter > 2 || path.empty()) {
		status_code = 500;
		send_body.clear();
		send_body = MESSAGE_LOOP;
		return ;
	}

	if (pathIsFile(path)) {
		file.open(path.c_str(), std::ifstream::in);
		if (!file.is_open()) {
			status_code = 403;
			readContent(getPathStatusCode());
			logger(STDOUT_FILENO, ERROR, "Cannot open file: " + path);
			return ;
		}
		buffer << file.rdbuf();
		send_body.clear();
		send_body = buffer.str();
		file.close();
		logger(STDOUT_FILENO, DEBUG, "File content loaded, size: " + stringify(send_body.size()));
	} else {
		struct stat s;
		if (stat(path.c_str(), &s) == 0 && S_ISDIR(s.st_mode)) {
			ListDirectory(path, request.uri);
		} else {
			status_code = 404;
			readContent(getPathStatusCode());
		}
	}
}

void Response::writeContent(const std::string &path, std::string content)
{
	std::ofstream	file;

	if (pathIsFile(path))
	{
		file.open(path.c_str());
		file << content;
		file.close();
		status_code = 204;
		readContent(getPathStatusCode());
	} else {
		file.open(path.c_str(), std::ofstream::out | std::ofstream::trunc);
		if (file.is_open() == false)
		{
			status_code = 403;
			readContent(getPathStatusCode());
			return ;
		}
		file << content;
		file.close();
		status_code = 201;
		readContent(getPathStatusCode());
	}
}

void Response::deleteContent(const std::string &path) {
	if (pathIsFile(path))
	{
		if (!remove(path.c_str())) {
			status_code = 204;
			readContent(getPathStatusCode());
		} else {
			status_code = 403;
			readContent(getPathStatusCode());
		}
	} else {
		status_code = 404;
		readContent(getPathStatusCode());
	}
}

void Response::ListDirectory(const std::string& path, const std::string& uri) {

	DIR* dir = opendir(path.c_str());
	if (!dir) {
		status_code = 404;
		readContent(getPathStatusCode());
		return ;
	}

	std::ostringstream listing;
	listing << "<html><head><title>Index of " << uri << "</title></head><body><h1>Index of " << uri << "</h1><hr><ul>";
	struct dirent* entry;
	while ((entry = readdir(dir)) != NULL) {
		std::string name = entry->d_name;
		if (name != "." && name != "..") {
			listing << "<li><a href=\"" << uri;
			if (uri[uri.length() - 1] != '/')
				listing << "/";
			listing << name << "\">" << name << "</a></li>";
		}
	}
	listing << "</ul><hr></body></html>";
	closedir(dir);
	send_body.clear();
	send_body.append(listing.str());
}
