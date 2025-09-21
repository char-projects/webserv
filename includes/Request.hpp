#ifndef REQUEST_HPP
# define REQUEST_HPP

# include <string>
# include <vector>
# include "Webserv.hpp"
# include "utils.hpp"

class Response;

class Request {

	public:
		Request(const int client_fd);
		~Request();

		void		setRecvData(const std::string& src_recv_data, size_t bytes_read);
		size_t		getBytesRecv() const;
		void		parseRecvData();
		bool		setSendData();

		// !!!!!!!!!!!!!!!!! BORRAR SIMULACIÓN

		std::string body;
		std::string header;
		std::vector<std::string>	valid_methods;
		std::string	method;
		std::string uri;
		std::string path;
		std::string location;
		ssize_t		status_code;

		// !!!!!!!!!!!!!!!!! BORRAR SIMULACIÓN

	private:
		int			client_fd;
		std::string	recv_data;
		size_t		bytes_recv;

		Request();
		Request(const Request& obj);
		Request &operator=(const Request& obj);
};

#endif
