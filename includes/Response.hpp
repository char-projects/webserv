#ifndef RESPONSE_HPP
#define RESPONSE_HPP

# include <string>
# include <cstring>

class Response {

	public:
        Response(const int client_fd);
        ~Response();

		void				setSendData(const std::string& src_send_data);
		const char*			getSendData() const;
		size_t				getBytesToSend() const;

	private:
        Response();
		Response(const Response &obj);
		Response &operator=(const Response &obj);

		std::string	send_data;
		int			client_fd;
		size_t		bytes_send;
};

#endif
