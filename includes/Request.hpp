#ifndef REQUEST_HPP
#define REQUEST_HPP

# include <string>
# include "Webserv.hpp"

class Response;

class Request {

	public:
        Request(const int client_fd, Response &response);
        ~Request();

		void		setRecvData(const std::string& src_recv_data, size_t bytes_read);
		size_t		getBytesRecv() const;
		void		parseRecvData();
		bool		setSendData();

	private:
        Request();
		Request(const Request& obj);
		Request &operator=(const Request& obj);

		int			client_fd;
		Response&	response;
		std::string	recv_data;
		size_t		bytes_recv;

};

#endif
