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
		HttpContext(const Event::Pool& events, int fd);
		~HttpContext();

		bool	loop(); // Event loop
		bool	handleRequest(int socket); // handle HTTP request
		bool	handleResponse(int socket); // handle HTTP response

	private:
		HttpContext();
		HttpContext(const HttpContext&);
		HttpContext&	operator=(const HttpContext&);

		Request		_request;
		Response	_response;
		const Event	_multiplexing;
};

std::ostream&	operator<<(std::ostream& out, const Request& req) {

}

std::ostream&	operator<<(std::ostream& out, const Request& res) {
	
}
#endif