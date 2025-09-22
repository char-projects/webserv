#ifndef REQUEST_HPP
#define REQUEST_HPP

# include <string>
# include <iostream>
# include <sstream>
# include <vector>
# include "Webserv.hpp"

class Request {
	private:
		std::map<std::string, std::string>	parameters;
		std::map<std::string, std::string>	headers;
		std::vector<std::string>	valid_methods;

		std::string	method;
		std::string path;
		std::string	uri;
		std::string	http_version;
		std::string body;
		ssize_t		status_code;
		int			client_fd;
		std::string	recv_data;

	public:
		Request(int client_fd);
		Request(const Request &other);
		Request &operator=(const Request &other);
		~Request();

		void parseParameters(const std::string &param_str);
		void setRecvData(const std::string& src_recv_data, size_t bytes_read);
		size_t getBytesRecv() const;
		void parseRecvData();
		bool setSendData();

		void setClientFd(int fd);
		int getClientFd() const;
		void setMethod(const std::string &method);
		std::string getMethod() const;
		void setPath(const std::string &path);
		std::string getPath() const;
		void setHttpVersion(const std::string &http_version);
		std::string getHttpVersion() const;
		void setBody(const std::string &body);
		std::string getBody() const;
		void setStatusCode(ssize_t code);
		ssize_t getStatusCode() const;
		void setUri(const std::string &uri);
		std::string getUri() const;
};

#endif
