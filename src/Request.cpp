#include "../includes/Request.hpp"
#include "../includes/utils.hpp"

Request::Request(int client_fd, const ServerConfig& config) : config(config) {
    this->client_fd = client_fd;
	status_code = 200;
	method = "GET";
	valid_methods.push_back("GET");
	valid_methods.push_back("POST");
	valid_methods.push_back("DELETE");
	valid_methods.push_back("HEAD");
	valid_methods.push_back("UNKNOWN");
	path = "";
    uri = "http://localhost:8080";
	http_version = "";
	body = "";
    recv_data = "";
	isMultipartFormData = false;
	boundary = "";
}

Request::Request(const Request &other) : config(other.config) {
	client_fd = other.client_fd;
	parameters = other.parameters;
	headers = other.headers;
	method = other.method;
	path = other.path;
	http_version = other.http_version;
	body = other.body;
}

Request &Request::operator=(const Request &other) {
	if (this != &other) {
		client_fd = other.client_fd;
		parameters = other.parameters;
		headers = other.headers;
		method = other.method;
		path = other.path;
		http_version = other.http_version;
		body = other.body;
	}
	return *this;
}

Request::~Request() {}

void Request::setClientFd(int fd) {
	this->client_fd = fd;
}

int Request::getClientFd() const {
	return client_fd;
}

void Request::setMethod(const std::string &method) {
	this->method = method;
}

std::string Request::getMethod() const {
	return method;
}

void Request::setPath(const std::string &path) {
	this->path = path;
}

std::string Request::getPath() const {
	return path;
}

void Request::setHttpVersion(const std::string &http_version) {
	this->http_version = http_version;
}

std::string Request::getHttpVersion() const {
	return http_version;
}

void Request::setBody(const std::string &body) {
	this->body = body;
}

std::string Request::getBody() const {
	return body;
}

void Request::setStatusCode(ssize_t code) {
    this->status_code = code;
}

ssize_t Request::getStatusCode() const {
    return status_code;
}

void Request::setUri(const std::string &uri) {
    this->uri = uri;
}

std::string Request::getUri() const {
    return uri;
}

void Request::parseParameters(const std::string &param_str) {
	size_t start = 0;
	size_t end = param_str.find('&');

	while (end != std::string::npos) {
		std::string param = param_str.substr(start, end - start);
		size_t eq_pos = param.find('=');
		if (eq_pos != std::string::npos) {
			std::string key = trim(param.substr(0, eq_pos));
			std::string value = trim(param.substr(eq_pos + 1));
			parameters[key] = value;
		}
		start = end + 1;
		end = param_str.find('&', start);
	}

	// Last parameter
	std::string param = param_str.substr(start);
	size_t eq_pos = param.find('=');
	if (eq_pos != std::string::npos) {
		std::string key = trim(param.substr(0, eq_pos));
		std::string value = trim(param.substr(eq_pos + 1));
		parameters[key] = value;
	}
}

bool Request::setSendData() {
    if (method == "UNKNOWN" || status_code != 200)
        return false;
    return true;
}

void Request::parseRecvData() {

    parameters.clear();
    headers.clear();
    body.clear();
    isMultipartFormData = false;
    boundary.clear();
    uploadedFiles.clear();

    if (recv_data.empty()) {
        status_code = 400;
        return;
    }

    std::istringstream request_stream(recv_data);
    std::string line;

    // Parse request line
    std::string method_local, uri_local, http_version_local;
    if (std::getline(request_stream, line)) {
        std::istringstream line_stream(line);
        if (!(line_stream >> method_local >> uri_local >> http_version_local)) {
            status_code = 400;
            return;
        }
    } else {
        status_code = 400;
        return;
    }

    // Validate method
    bool valid_method = false;
    for (size_t i = 0; i < valid_methods.size(); ++i) {
        if (method_local == valid_methods[i]) {
            valid_method = true;
            break;
        }
    }
    if (!valid_method) {
        method = "UNKNOWN";
        status_code = 405;
        return;
    }

    size_t query_pos = uri_local.find('?');
	std::string clean_uri = (query_pos != std::string::npos) ? uri_local.substr(0, query_pos) : uri_local;
	clean_uri = normalizePath(clean_uri);

	uri = clean_uri;
	if (query_pos != std::string::npos) {
		std::string param_str = uri_local.substr(query_pos + 1);
		parseParameters(param_str);
	}

	method = method_local;
	http_version = http_version_local;
	std::string serverRoot = config.getRoot();
	if (serverRoot.empty())
		serverRoot = "www";

	serverRoot = normalizePath(serverRoot);

	if (!serverRoot.empty() && serverRoot[serverRoot.length()-1] == '/') {
		serverRoot = serverRoot.substr(0, serverRoot.length()-1);
	}

	if (clean_uri == "/") {
		path = serverRoot;
		if (!config.getIndexFiles().empty()) {
			path += "/" + config.getIndexFiles()[0];
		} else {
			path += "/index.html";
		}
	} else {
		path = serverRoot + clean_uri;
	}
	path = normalizePath(path);

    // Parse headers
    while (std::getline(request_stream, line) && line != "\r") {
        size_t colon_pos = line.find(':');
        if (colon_pos != std::string::npos) {
            std::string header_name = trim(line.substr(0, colon_pos));
            std::string header_value = trim(line.substr(colon_pos + 1));
            headers[header_name] = header_value;
        }
    }

    // The rest is the body
    std::string body_content;
    while (std::getline(request_stream, line)) {
        body_content += line + "\n";
    }
    if (!body_content.empty() && body_content[body_content.size() - 1] == '\n')
        body_content.erase(body_content.size() - 1); // Remove the last newline character
    body = body_content;

	if (method == "POST")
		parseMultipartFormData();

    for (std::map<std::string, std::string>::iterator it = parameters.begin(); it != parameters.end(); ++it) {
        logger(STDOUT_FILENO, INFO, "  " + it->first + " = " + it->second);
    }
    logger(STDOUT_FILENO, INFO, "Body:\t" + body);
    logger(STDOUT_FILENO, INFO, "Headers:");
    for (std::map<std::string, std::string>::iterator it = headers.begin(); it != headers.end(); ++it) {
        logger(STDOUT_FILENO, INFO, "  " + it->first + ": " + it->second);
    }
    status_code = 200;

	if (headers.count("Transfer-Encoding") && headers["Transfer-Encoding"] == "chunked")
		body = decodeChunked(body_content);
	else
		body = body_content;
}

void Request::setRecvData(const std::string& src_recv_data, size_t bytes_read) {
	if (bytes_read <= 0 || bytes_read > BUFFER_RECV_SIZE) {
		logger(STDOUT_FILENO, ERROR, "Error reading from client or connection closed");
        status_code = 400;
		return ;
	}
    if (!recv_data.empty()) {
        parseRecvData();
        recv_data.clear();
    }
	recv_data.append(src_recv_data);
    if (recv_data.find("\r\n\r\n") != std::string::npos)
        parseRecvData();
}

std::string Request::decodeChunked(const std::string &chunkedBody) {
	std::istringstream	stream(chunkedBody);
	std::string			decoded;
	std::string			line;

	while (std::getline(stream, line)) {
		if (!line.empty() && line[line.size() - 1] == '\r')
			line.erase(line.size() - 1);

		std::istringstream hexSize(line);
		size_t chunkSize = 0;
		hexSize >> std::hex >> chunkSize;

		if (!chunkSize)
			break;

		std::vector<char> buffer(chunkSize);
		stream.read(buffer.data(), chunkSize);
		decoded.append(buffer.begin(), buffer.end());
		stream.ignore(2);
	}
	return (decoded);
}

size_t Request::getBytesRecv() const {
	return (recv_data.size());
}

std::map<std::string, std::string> Request::getParameters() const {
    return parameters;
}

std::map<std::string, std::string> Request::getHeaders() const {
    return headers;
}

bool Request::isMultipart() const {
	return (isMultipartFormData);
}

std::string Request::getBoundary() const {
	return (boundary);
}

std::map<std::string, std::string> Request::getUploadedFiles() const {
	return (uploadedFiles);
}

void Request::parseMultipartFormData() {
	if (!headers.count("Content-Type"))
		return ;

	std::string contentType = headers["Content-Type"];
	size_t boundaryPos = contentType.find("boundary=");

	if (boundaryPos != std::string::npos) {
		isMultipartFormData = true;
		boundary = contentType.substr(boundaryPos + 9);
		if (boundary[0] == '"' && boundary[boundary.length()-1] == '"')
			boundary = boundary.substr(1, boundary.length()-2);
	}

	if (!isMultipartFormData || boundary.empty())
		return ;

	std::string fullBoundary = "--" + boundary;
	size_t pos = 0;
	while ((pos = body.find(fullBoundary, pos)) != std::string::npos) {
		size_t partStart = pos + fullBoundary.length();
		if (body[partStart] == '\r' && body[partStart+1] == '\n')
			partStart += 2;
		else if (body.substr(partStart, 2) == "--")
			break;

		size_t partEnd = body.find(fullBoundary, partStart);
		if (partEnd == std::string::npos)
			break;

		std::string part = body.substr(partStart, partEnd - partStart);

		size_t headerEnd = part.find("\r\n\r\n");
		if (headerEnd != std::string::npos) {
			std::string partHeaders = part.substr(0, headerEnd);
			std::string partBody = part.substr(headerEnd + 4);
			size_t namePos = partHeaders.find("name=\"");
			size_t filenamePos = partHeaders.find("filename=\"");

			if (namePos != std::string::npos && filenamePos != std::string::npos) {
				size_t nameStart = namePos + 6;
				size_t nameEnd = partHeaders.find("\"", nameStart);
				std::string fieldName = partHeaders.substr(nameStart, nameEnd - nameStart);
				size_t filenameStart = filenamePos + 10;
				size_t filenameEnd = partHeaders.find("\"", filenameStart);
				std::string filename = partHeaders.substr(filenameStart, filenameEnd - filenameStart);
				uploadedFiles[fieldName] = partBody;
				logger(STDOUT_FILENO, INFO, "Uploaded file: " + filename + " for field: " + fieldName + " size: " + stringify(partBody.size()));
			}
		}
		pos = partEnd;
	}
}

void Request::reset() {
	status_code = 200;
	method = "GET";
	path = "";
	uri = "http://localhost:8080";
	http_version = "";
	body = "";
	recv_data = "";
	parameters.clear();
	headers.clear();
	isMultipartFormData = false;
	boundary = "";
	uploadedFiles.clear();
}