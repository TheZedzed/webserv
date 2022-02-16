#pragma once

# ifndef HTTPCONTEXT_HPP
# define HTTPCONTEXT_HPP

# include "Event.hpp"

struct	Request {
	String	request_line;
	Fields	header_fields;
	String	body;
};

struct	Response {
	String	status_line;
	Fields	header_fields;
	String	body;
};

class	HttpContext {
	public:
		HttpContext();
		~HttpContext(); // destroy Event

		bool	configure(const String& file); // parse config file
		bool	loop(); // Event loop
		bool	handleRequest(int socket); // handle HTTP request
		bool	handleResponse(int socket); // handle HTTP response
	private:
		HttpContext(const HttpContext&);
		HttpContext&	operator=(const HttpContext&);

		Event*		_multiplexing;
		Request		_request;
		Response	_response;
};
#endif