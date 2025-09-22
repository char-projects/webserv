#include "../includes/Request.hpp"

Request::Request(const int client_fd) {
	logger(STDOUT_FILENO, DEBUG, "Constructor Request called");
	this->client_fd = client_fd;
	recv_data.clear();
	bytes_recv = 0;

	// !!!!!!!!!!!!!!!!! BORRAR SIMULACIÓN

	body = ".................";
	header = "...............";
	uri = "https://localhost:8080/index.html#something";
	path = "www/index.html";
	location = "";
	status_code = 500;
	method = "GET";

	valid_methods.push_back("GET");
	valid_methods.push_back("POST");
	valid_methods.push_back("DELETE");
	valid_methods.push_back("UNKNOWN");

	//uri = "localhost:8080/index.php?params=2423424";

	// !!!!!!!!!!!!!!!!! BORRAR SIMULACIÓN
}

Request::~Request() {
	logger(STDOUT_FILENO, DEBUG, "Destructor Request called");
}

void Request::setRecvData(const std::string& src_recv_data, size_t bytes_read) {

	if (bytes_read <= 0 || bytes_read > BUFFER_SIZE) {
		// TODO error
		return ;
	}

	recv_data.append(src_recv_data);

	std::string request_log = "Received (" + stringify(bytes_read) + " bytes) from client " + stringify(client_fd);
	logger(STDOUT_FILENO, SUCCESS, request_log);
	logger(STDOUT_FILENO, SUCCESS, recv_data);
	parseRecvData();
}

size_t Request::getBytesRecv() const {
	return (recv_data.size());
}

void Request::parseRecvData() {

    if (recv_data.empty()) {
        status_code = 400;
        return ;
    }

    size_t first_line_end = recv_data.find("\r\n");
    if (first_line_end == std::string::npos) {
        first_line_end = recv_data.find("\n");
        if (first_line_end == std::string::npos) {
            status_code = 400;
            return ;
        }
    }

    std::string request_line = recv_data.substr(0, first_line_end);
    std::istringstream iss(request_line);
    std::string http_version;
    if (!(iss >> method >> uri >> http_version)) {
        status_code = 400;
        return ;
    }

    bool valid_method = false;
    for (size_t i = 0; i < valid_methods.size(); ++i) {
        if (method == valid_methods[i]) {
            valid_method = true;
            break;
        }
    }

    if (!valid_method) {
        method = "UNKNOWN";
        status_code = 405;
        return ;
    }

    size_t query_pos = uri.find('?');
    std::string clean_uri = (query_pos != std::string::npos) ? uri.substr(0, query_pos) : uri;
    path = "www" + clean_uri;
    if (clean_uri == "/")
        path = "www";

    logger(STDOUT_FILENO, INFO, "Method:\t" + method);
    logger(STDOUT_FILENO, INFO, "URI:\t" + uri);
    logger(STDOUT_FILENO, INFO, "Path:\t" + path);
    status_code = 200;
}

bool Request::setSendData() {
	return (true);
}
