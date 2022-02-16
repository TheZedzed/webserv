#pragma once

# ifndef HTTPCONTEXT_HPP
# define HTTPCONTEXT_HPP

# include "Event.hpp"

typedef struct	s_Request {
	std::string							request_line;
	std::map<std::string, std::string>	header_fields;
	std::string							body;
}t_Request;

typedef struct	s_Response {
	std::string							status_line;
	std::map<std::string, std::string>	header_fields;
	std::string							body;
}t_Response;

class	HttpContext {
	public:
		HttpContext();
		~HttpContext();// destroy Event
		bool	configure(const std::string& file);
		bool	loop();
		bool	handleRequest(int socket);
		bool	handleResponse(int socket);
	private:
		HttpContext(const HttpContext&);
		HttpContext&	operator=(const HttpContext&);

		Event*		_multiplexing;
		t_Request	_request;
		t_Response	_response;
};
#endif