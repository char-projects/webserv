#include "../includes/Response.hpp"
#include "../includes/Request.hpp"
#include "../includes/utils.hpp"

Response::Response(const int client_fd, const Request& request, const ServerConfig& config, const std::vector<LocationConfig*>& locations)
	: client_fd(client_fd), request(request), config(config), locations(locations) {
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
	counter = 0;

	status_code = request.getStatusCode();
	logger(STDOUT_FILENO, SUCCESS, "Host: " + config.getHost() +
		" Request:  " + request.getMethod() + " " + request.getPath() + " " +
		request.getHttpVersion() + " " + stringify(status_code));

	// ----> CGI logger(STDOUT_FILENO, SUCCESS, "Params: " + request.getParams();

	if (status_code == 200) {
		if (request.getMethod() == "GET" || request.getMethod() == "HEAD") {
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
	if (request.getMethod() != "HEAD")
		send_response.append(send_body);

	if (status_code > 299 && request.getMethod() != "HEAD")
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

	std::string error_page_path;

	error_page_path.clear();
	switch (status_code) {
		case 201:
			error_page_path = "";
			break;
		case 204:
			error_page_path = "";
			break;
		case 301:
			error_page_path = "";
			break;
		case 307:
			error_page_path = "";
			break;
		case 400:
			error_page_path = "src/error_pages/400.html";
			break;
		case 403:
			error_page_path = "src/error_pages/403.html";
			break;
		case 404:
			error_page_path = "src/error_pages/404.html";
			break;
		case 405:
			error_page_path = "src/error_pages/405.html";
			break;
		case 413:
			error_page_path = "src/error_pages/413.html";
			break;
		case 500:
			error_page_path = "src/error_pages/500.html";
			break;
		case 502:
			error_page_path = "src/error_pages/502.html";
			break;
		case 503:
			error_page_path = "src/error_pages/503.html";
			break;
		case 504:
			error_page_path = "src/error_pages/504.html";
			break;
		case 520:
			error_page_path = "";
			break;
		default:
			error_page_path = "src/error_pages/500.html";
	}

	std::vector<std::pair<std::string, int> > pairs = config.getErrorPages();
	for (std::vector<std::pair<std::string, int> >::const_iterator it = pairs.begin(); it != pairs.end(); ++it) {
		if (it->second == static_cast<int>(status_code)) {
			error_page_path = it->first;
			break;
		}
	}
	return (error_page_path);
}

void Response::readContent(const std::string &path) {

	if (++counter > 2) {
		status_code = 500;
		send_body.clear();
		send_body = MESSAGE_LOOP;
		return ;
	}

	std::string request_uri = request.getUri();
	LocationConfig* loc =  findLocation(request.getUri());
	if (loc && !loc->getRedirects().empty()) {
		std::vector<std::pair<std::string, std::string> > redirects = loc->getRedirects();
		if (!redirects.empty()) {
			status_code = 301;
			response_header->setLocation(redirects[0].second);
			send_body.clear();
			return ;
		}
	}

	PathType result = checkPath(path);
	std::ifstream file;
	std::stringstream buffer;
	std::string index_file;

	if (result == PATH_IS_DIRECTORY && path[path.size()-1] != '/') {
		status_code = 301;
		response_header->setLocation(request.getUri() + "/");
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
			if (!config.getIndexFiles().empty()) {
				index_file = path + "/" + config.getIndexFiles()[0];
				if (pathIsFile(index_file)) {
					readContent(index_file);
					break;
				}
			}
			loc = findLocation(request.getUri());
			if (loc && loc->getAutoIndex()) {
				ListDirectory(path, request.getUri());
			} else {
				status_code = 403;
				readContent(getPathStatusCode());
			}
			break;

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

	if (request.isMultipart() && !request.getUploadedFiles().empty()) {
		handleFileUpload(content);
		return ;
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

void Response::handleFileUpload(const std::string &content) {
	(void) content;
	std::string uploadDir = config.getUploadPath();
	if (!pathIsDirectory(uploadDir)) {
		if (mkdir(uploadDir.c_str(), 0755) != 0 && errno != EEXIST) {
			status_code = 500;
			readContent(getPathStatusCode());
			logger(STDOUT_FILENO, ERROR, "Cannot create upload directory: " + uploadDir);
			return ;
		}
	}

	const std::map<std::string, std::string>& uploadedFiles = request.getUploadedFiles();
	std::string responseBody = "<html><body><h1>File Upload Results</h1><ul>";
	bool success = false;
	for (std::map<std::string, std::string>::const_iterator it = uploadedFiles.begin(); it != uploadedFiles.end(); ++it) {
		std::string filename = it->first + "_upload_" + stringify(time(NULL)) + ".dat";
		std::string fullPath = uploadDir + "/" + filename;

		std::ofstream file(fullPath.c_str(), std::ios::binary);
		if (file.is_open()) {
			file.write(it->second.c_str(), it->second.size());
			file.close();
			responseBody += "<li>File '" + it->first + "' uploaded successfully as: " + filename + "</li>";
			success = true;
			logger(STDOUT_FILENO, SUCCESS, "File uploaded: " + fullPath + " size: " + stringify(it->second.size()));
		} else {
			responseBody += "<li>Failed to upload file: " + it->first + "</li>";
			logger(STDOUT_FILENO, ERROR, "Failed to write uploaded file: " + fullPath);
		}
	}

	responseBody += "</ul></body></html>";

	if (success) {
		status_code = 201;
		send_body = responseBody;
	} else {
		status_code = 500;
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
	listing << "<html><head><title>Index of " << uri << "</title><link rel=\"stylesheet\" href=\"error_pages/styles.css\"></head><body><h1>Index of " << uri << "</h1><hr><ul>";
	struct dirent* entry;
	if (uri != "/") {

		std::string tmp = uri;
		if (tmp.length() > 1 && tmp[tmp.length() - 1] == '/')
			tmp.erase(tmp.length() - 1);

		size_t pos = tmp.find_last_of('/');
		std::string parent_uri;
		if (pos == std::string::npos || pos == 0)
			parent_uri = "/";
		else
			parent_uri = tmp.substr(0, pos) + "/";

		listing << "<li><a href=\"" << parent_uri << "\">../</a></li>";
	}

	while ((entry = readdir(dir)) != NULL) {
		std::string name = entry->d_name;
		if (name != "." && name != "..") {

			std::string file_uri = uri;
			if (file_uri[file_uri.length() - 1] != '/')
				file_uri += "/";
			file_uri += name;

			std::string full_path = path;
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

LocationConfig* Response::findLocation(const std::string& uri) {
	LocationConfig* matching_location = NULL;
	size_t best_match_length = 0;

	for (std::vector<LocationConfig *>::const_iterator it = locations.begin(); it != locations.end(); ++it) {
		std::string location_path = (*it)->getLocationPath();

		if (uri == location_path ||
			uri.find(location_path + "/") == 0 ||
			(location_path != "/" && uri.find(location_path) == 0)) {

			if (location_path.length() > best_match_length) {
				best_match_length = location_path.length();
				matching_location = *it;
			}
		}
	}
	return (matching_location);
}

size_t Response::getStatusCode() const {
	return (status_code);
}

void Response::reset() {
	send_response.clear();
	send_header.clear();
	send_body.clear();
	status_code = 200;
	bytes_send = 0;
	counter = 0;
}