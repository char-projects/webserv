#include "../includes/Request.hpp"
#include "../includes/utils.hpp"

Request::Request(const int client_fd, Response& response) : client_fd(client_fd), response(response) {
	logger(STDOUT_FILENO, DEBUG, "Constructor Request called");
	recv_data.clear();
	bytes_recv = 0;
}

Request::~Request() {
	logger(STDOUT_FILENO, DEBUG, "Destructor Request called");
}

void Request::setRecvData(const std::string& src_recv_data, size_t bytes_read) {


    if (bytes_read <= 0 || bytes_read > BUFFER_SIZE) {
		// TODO error
        return ;
    }

    recv_data.append(src_recv_data, bytes_read);

	std::string request_log = "Received (" + stringify(bytes_read) + " bytes) from client " + stringify(client_fd);
	logger(STDOUT_FILENO, SUCCESS, request_log);
	logger(STDOUT_FILENO, SUCCESS, recv_data);
	// TODO --------------------
	parseRecvData();
}


size_t Request::getBytesRecv() const {
	return (recv_data.size());
}


void Request::parseRecvData() {

}

bool Request::setSendData() {

	response.setSendData(recv_data);
	return (true);
}
