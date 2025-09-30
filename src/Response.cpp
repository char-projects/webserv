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
				logger(STDOUT_FILENO, ERROR, getPathStatusCode());
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
	(void)content;

	logger(STDOUT_FILENO, INFO, "=== START FILE UPLOAD HANDLING ===");

	const std::string &body_content = request.getBody();
	logger(STDOUT_FILENO, INFO, "Body size for upload: " + stringify(body_content.size()));


	std::string firstBytes;
	for (size_t i = 0; i < body_content.size() && i < 100; ++i) {
		char c = body_content[i];
		if (c >= 32 && c <= 126) {
			firstBytes += c;
		} else {
			char hex[5];
			snprintf(hex, sizeof(hex), "\\x%02X", (unsigned char)c);
			firstBytes += hex;
		}
	}
	logger(STDOUT_FILENO, INFO, "First 100 bytes (hex for non-printable): " + firstBytes);


	const std::map<std::string, std::string>& uploadedFiles = request.getUploadedFiles();
	logger(STDOUT_FILENO, INFO, "Files parsed by request: " + stringify(uploadedFiles.size()));
	for (std::map<std::string, std::string>::const_iterator it = uploadedFiles.begin(); it != uploadedFiles.end(); ++it) {
		logger(STDOUT_FILENO, INFO, " - " + it->first + " : " + stringify(it->second.size()) + " bytes");
	}


	std::string uploadPathConfig = config.getUploadPath();
	std::string serverRoot = config.getRoot();
	if (serverRoot.empty()) serverRoot = "www";

	if (!serverRoot.empty() && serverRoot[serverRoot.size() - 1] == '/')
		serverRoot.erase(serverRoot.size() - 1);

	std::string uploadDir;
	if (!uploadPathConfig.empty() && uploadPathConfig[0] == '/')
		uploadDir = uploadPathConfig;
	else {
		uploadDir = serverRoot;
		if (!uploadDir.empty()) uploadDir += "/";
		uploadDir += uploadPathConfig;
	}


	size_t p;
	while ((p = uploadDir.find("//")) != std::string::npos) uploadDir.erase(p, 1);

	logger(STDOUT_FILENO, INFO, "Upload directory: " + uploadDir);


	struct stat st;
	if (stat(uploadDir.c_str(), &st) != 0) {
		logger(STDOUT_FILENO, INFO, "Upload directory doesn't exist, creating: " + uploadDir);
		if (mkdir(uploadDir.c_str(), 0755) != 0 && errno != EEXIST) {
			status_code = 500;
			logger(STDOUT_FILENO, ERROR, "Cannot create upload directory: " + uploadDir + " errno: " + stringify(errno));
			readContent(getPathStatusCode());
			return;
		}
	} else {
		if (!S_ISDIR(st.st_mode)) {
			status_code = 500;
			logger(STDOUT_FILENO, ERROR, "Upload path exists but is not a directory: " + uploadDir);
			readContent(getPathStatusCode());
			return;
		}
	}


	if (!uploadedFiles.empty()) {
		logger(STDOUT_FILENO, INFO, "Using files parsed by Request class");
		bool anyWritten = false;
		std::string responseBody = "<html><body><h1>File Upload Results</h1><ul>";

		for (std::map<std::string, std::string>::const_iterator it = uploadedFiles.begin(); it != uploadedFiles.end(); ++it) {
			std::string filename = it->first;
			const std::string& fileData = it->second;

			std::string fullPath = uploadDir;
			if (fullPath.empty() || fullPath[fullPath.size() - 1] != '/')
				fullPath += "/";
			fullPath += filename;

			logger(STDOUT_FILENO, INFO, "Writing file: " + fullPath + " size: " + stringify(fileData.size()));

			std::ofstream ofs(fullPath.c_str(), std::ios::binary | std::ios::out | std::ios::trunc);
			if (!ofs.is_open()) {
				logger(STDOUT_FILENO, ERROR, "Failed to open file for writing: " + fullPath);
				responseBody += "<li>Failed to write file: " + filename + "</li>";
			} else {
				ofs.write(fileData.data(), static_cast<std::streamsize>(fileData.size()));
				ofs.close();


				struct stat fileStat;
				if (stat(fullPath.c_str(), &fileStat) == 0) {
					logger(STDOUT_FILENO, SUCCESS, "File written successfully: " + fullPath + " size: " + stringify(fileStat.st_size));
				} else {
					logger(STDOUT_FILENO, ERROR, "File write verification failed for: " + fullPath);
				}

				responseBody += "<li>Uploaded: " + filename + " (" + stringify(fileData.size()) + " bytes)</li>";
				anyWritten = true;
			}
		}

		responseBody += "</ul></body></html>";

		if (anyWritten) {
			status_code = 201;
			send_body = responseBody;
		} else {
			status_code = 400;
			readContent(getPathStatusCode());
		}
		return;
	}

	logger(STDOUT_FILENO, WARNING, "No files parsed by Request, falling back to manual parsing");


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