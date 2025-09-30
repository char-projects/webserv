#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include <string>
# include <cstring>
// ListDirectory
# include <dirent.h>

# include "Webserv.hpp"
# include "ResponseHeader.hpp"
# include "ServerConfig.hpp"
# include "LocationConfig.hpp"
# include "Request.hpp"

# define MESSAGE_LOOP "<html><body><h1>500 Internal Server Error</h1></body></html>"

class ServerConfig;
class Request;
class ResponseHeader;
class LocationConfig;

class Response {

	public:
		Response(const int client_fd, const Request& request, const ServerConfig& config, const std::vector<LocationConfig*>& locations);
		~Response();
		const char*			getResponse();
		size_t				getSize();
		size_t				getStatusCode() const;

	private:
		std::string							send_response;
		std::string							send_header;
		std::string							send_body;
		int									client_fd;
		size_t								status_code;
		size_t								bytes_send;
		size_t								counter;
		const Request&						request;
		const ServerConfig&					config;
		ResponseHeader*						response_header;
 		const std::vector<LocationConfig*>&	locations;

		Response();
		Response(const Response &obj);
		Response &operator=(const Response &obj);

		void				readContent(const std::string &path);
		void				writeContent(const std::string &path, std::string content);
		void				handleFileUpload(const std::string &content);
		void				deleteContent(const std::string &path);
		void		 		ListDirectory(const std::string& path, const std::string& uri);
		const std::string	getPathStatusCode();
		LocationConfig*		findLocation(const std::string& path);
		void				reset();




};

#endif