#include "../includes/Response.hpp"
#include "../includes/Request.hpp"
#include "../includes/utils.hpp"

Response::Response(const int client_fd, const Request& request, const ServerConfig& config)
	: client_fd(client_fd), request(request), config(config) {
	send_response.clear();
	send_header.clear();
	send_body.clear();
	status_code = 200;
	bytes_send = 0;
	counter = 0;
	response_header = new ResponseHeader();
}

Response::~Response() {
	delete response_header;
}

const char* Response::getResponse() {
	e_message log_type = SUCCESS;

	status_code = request.getStatusCode();
	logger(STDOUT_FILENO, SUCCESS, "Host: " + config.getHost() +
		" Request: " + request.getMethod() + " " + request.getPath() + " " +
		request.getHttpVersion() + " " + stringify(status_code));

	// ----> CGI logger(STDOUT_FILENO, SUCCESS, "Params: " + request.getParams();

	// ----> 	if request.getRedirects(request.getPath())  and .....

	if (status_code == 200) {
		if (request.getMethod() == "GET") {
			readContent(request.getPath());
		} else if (request.getMethod() == "POST") {
			writeContent(request.getPath(), request.getBody());
		} else if (request.getMethod() == "DELETE") {
			deleteContent(request.getPath());
		} else {
			status_code = 405;
			readContent(getPathStatusCode());
		}
	} else {
		readContent(getPathStatusCode());
	}

	response_header->setContentLength(send_body.size());
	response_header->setContentType(request.getPath());
	send_header = response_header->getHeader(status_code);
	send_response.clear();
	send_response.append(send_header);
	send_response.append(send_body);

	if (status_code > 299)
		log_type = ERROR;

	logger(STDOUT_FILENO, log_type, "Host: " + config.getHost() +
		" Response: " + request.getMethod() + " " + request.getPath() + " " +
		request.getHttpVersion() + " " + stringify(status_code) + " Size: " +
		stringify(send_body.size()) + "bytes");

	return (send_response.data());
}

size_t Response::getSize() {
	return (send_response.size());
}

const std::string Response::getPathStatusCode() {

	// ----> ServerConfig server = request.getLocationPath().getErrorPages();


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
		case 201: return ("");
		case 204: return ("");
		case 301: return ("");
		case 307: return ("");
		case 400: return ("src/error_pages/400.html");
		case 403: return ("src/error_pages/403.html");
		case 404: return ("src/error_pages/404.html");
		case 405: return ("src/error_pages/405.html");
		case 413: return ("src/error_pages/413.html");
		case 500: return ("src/error_pages/500.html");
		case 502: return ("src/error_pages/502.html");
		case 503: return ("src/error_pages/503.html");
		case 504: return ("src/error_pages/504.html");
		case 520: return ("");
		default: return ("src/error_pages/500.html");
	}

	// TEMPORAL DELETEME
	counter = 0;
}

void Response::readContent(const std::string &path) {

	if (++counter > 2) {
		status_code = 500;
		send_body.clear();
		send_body = MESSAGE_LOOP;
		return ;
	}

	/*
	if (path.empty() || path == "www")
	{
		// ----> path = request.location.getRoot() + request.location.getRoot().getIndexFiles
		//readContent("www/index.html");
		status_code = 301;
		response_header->setLocation(path + "/index.html");
		send_body.clear();
	}
	*/

	PathType result = checkPath(path);
	std::ifstream file;
	std::stringstream buffer;
	std::string index_file;

	if (result == PATH_IS_DIRECTORY && path[path.size()-1] != '/') {
		status_code = 301;
		response_header->setLocation(path + "/");
		send_body.clear();
		return ;
	}

	switch (result) {
		case PATH_IS_FILE:
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
			break;

		case PATH_IS_DIRECTORY:

			index_file = path + "/index.html";
			if (pathIsFile(index_file))
				readContent(index_file);
			else
				ListDirectory(path, request.getUri());
			break;

			// ---->  LocationConfig location = request.getLocationPath(path);

			// if (location != NULL && location.getAutoIndex())
				//ListDirectory(path, request.getUri());
			// else {
			//	status_code = 403;
			//	readContent(getPathStatusCode());
			//}

			//break;
		case PATH_NO_PERMISSION:

			if (status_code != 403) {
				status_code = 403;
				readContent(getPathStatusCode());
			}
			break;
		case PATH_NOT_EXISTS:
			if (status_code != 404) {
				status_code = 404;
				readContent(getPathStatusCode());
			}
			break;
		case PATH_IS_OTHER:
			if (status_code != 400) {
				status_code = 400;
				readContent(getPathStatusCode());
			}
			break;
		case PATH_ERROR:
			if (status_code != 400) {
				status_code = 400;
				readContent(getPathStatusCode());
			}
			break;
		default:
			if (status_code != 404) {
				status_code = 404;
				readContent(getPathStatusCode());
			} else {
				status_code = 520;
				send_body.clear();
				return ;
			}
	}
}

void Response::writeContent(const std::string &path, std::string content)
{
	if (content.empty()) {
		status_code = 400;
		readContent(getPathStatusCode());
	}

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

   	std::string str = path;

	std::string to_remove = "www/";
	size_t pos = str.find(to_remove);
	if (pos != std::string::npos)
		str.erase(pos, to_remove.length());

	if (uri != "/") {
		size_t last_slash = uri.find_last_of('/');
		std::string parent_uri = (last_slash > 0) ? uri.substr(0, last_slash) : "/";
		listing << "<li><a href=\"" << parent_uri << "\">../</a></li>";
	}

	while ((entry = readdir(dir)) != NULL) {
		std::string name = entry->d_name;
		if (name != "." && name != "..") {
			std::string file_uri = uri;
			if (file_uri[file_uri.length() - 1] != '/')
				file_uri += "/";
			file_uri += str + name;

			std::string full_path = str;
			if (full_path[full_path.length() - 1] != '/')
				full_path += "/";
			full_path += name;

			std::string display_name = name;
			if (pathIsDirectory(full_path))
				display_name += "/";

			listing << "<li><a href=\"" << file_uri << "\">" << display_name << "</a></li>";
		}
	}

	listing << "</ul><hr></body></html>";
	closedir(dir);
	send_body.clear();
	send_body.append(listing.str());
	status_code = 200;
}
