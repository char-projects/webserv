#include "../includes/Response.hpp"
#include "../includes/utils.hpp"

Response::Response(const int client_fd) : client_fd(client_fd) {
	logger(STDOUT_FILENO, DEBUG, "Constructor Response called");
	send_data.clear();
	bytes_send = 0;
}

Response::~Response() {
	logger(STDOUT_FILENO, DEBUG, "Destructor Response called");
}

void Response::setSendData(const std::string& src_send_data) {
	(void) src_send_data;
	std::string body = "<h1>Probando probando!!!!</h1>";
	std::string header = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: " + stringify(body.length()) + "\r\n\r\n";

	send_data = header + body;
}

const char* Response::getSendData() const {
	std::string log_msg = "Sent (" + stringify(send_data.size()) + " bytes) to client " + stringify(client_fd);
	logger(STDOUT_FILENO, SUCCESS, log_msg);
	return (send_data.data());
}

size_t Response::getBytesToSend() const {
	return (send_data.size());
}
