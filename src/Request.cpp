#include "../includes/Request.hpp"
#include "../includes/utils.hpp"

Request::Request(int client_fd) {
    this->client_fd = client_fd;
	status_code = 200;
	method = "GET";
	valid_methods.push_back("GET");
	valid_methods.push_back("POST");
	valid_methods.push_back("DELETE");
	valid_methods.push_back("UNKNOWN");
	path = "index.html";
    uri = "http://localhost:8080";
	http_version = "";
	body = "";
    recv_data = "";
}

Request::Request(const Request &other) {
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

    // Parse parameters from URI
    size_t query_pos = uri_local.find('?');
    std::string clean_uri = (query_pos != std::string::npos) ? uri_local.substr(0, query_pos) : uri_local;
    if (query_pos != std::string::npos) {
        std::string param_str = uri_local.substr(query_pos + 1);
        parseParameters(param_str);
    }

    // Set method, path, http_version
    method = method_local;
    http_version = http_version_local;
    path = "www" + clean_uri;
    if (clean_uri == "/")
        path = "www/";

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

    logger(STDOUT_FILENO, INFO, "Method:\t" + method);
    logger(STDOUT_FILENO, INFO, "URI:\t" + uri_local);
    logger(STDOUT_FILENO, INFO, "Path:\t" + path);
    logger(STDOUT_FILENO, INFO, "Version:\t" + http_version);
    logger(STDOUT_FILENO, INFO, "Parameters:");
    for (std::map<std::string, std::string>::iterator it = parameters.begin(); it != parameters.end(); ++it) {
        logger(STDOUT_FILENO, INFO, "  " + it->first + " = " + it->second);
    }
    logger(STDOUT_FILENO, INFO, "Body:\t" + body);
    logger(STDOUT_FILENO, INFO, "Headers:");
    for (std::map<std::string, std::string>::iterator it = headers.begin(); it != headers.end(); ++it) {
        logger(STDOUT_FILENO, INFO, "  " + it->first + ": " + it->second);
    }
    std::ostringstream oss;
    oss << status_code;
    logger(STDOUT_FILENO, INFO, "Status Code:\t" + oss.str());
    status_code = 200;




	// ################ AÑADIDO #########################

	if (headers.count("Transfer-Encoding") && headers["Transfer-Encoding"] == "chunked") {
		body = decodeChunked(body_content);
	} else {
		body = body_content;
	}

	// ################ AÑADIDO #########################


}

void Request::setRecvData(const std::string& src_recv_data, size_t bytes_read) {

	if (bytes_read <= 0 || bytes_read > BUFFER_RECV_SIZE) {
		logger(STDOUT_FILENO, ERROR, "Error reading from client or connection closed");
        status_code = 400;
		return ;
	}

	recv_data.append(src_recv_data);

	logger(STDOUT_FILENO, SUCCESS, recv_data);
	parseRecvData();
}


size_t Request::getBytesRecv() const {
	return (recv_data.size());
}


// ################ AÑADIDO #########################

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


// ################ AÑADIDO
