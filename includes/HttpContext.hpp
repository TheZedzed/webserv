#pragma once

# ifndef HTTPCONTEXT_HPP
# define HTTPCONTEXT_HPP

# include "Event.hpp"

struct	Request {
	String	request_line;
	std::map<String, String>	fields;
	String	body;
};

struct	Response {
	String	status_line;
	std::map<String, String>	fields;
	String	body;
};

class	HttpContext {
	public:
		HttpContext(const Event::Pool& events, int fd);
		~HttpContext();

		bool	loop(); // Event loop
		bool	handleRequest(int socket); // handle HTTP request
		bool	handleResponse(int socket); // handle HTTP response
		int		newConnection(int socket) const; // handle new connection

	private:
		HttpContext();
		HttpContext(const HttpContext&);
		HttpContext&	operator=(const HttpContext&);

		Request		_request;
		Response	_response;
		const Event	_multiplexing;
};
#endif