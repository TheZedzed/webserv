#pragma once

# ifndef HTTPCONTEXT_HPP
# define HTTPCONTEXT_HPP

# include "Event.hpp"
# include "Parser.hpp"

class	Request {
	public:
		typedef std::map<String, String>	Fields;

		~Request();
		Request(const String& raw) : _data(raw) {}

		int	parser();

	private:
		Request();
		Request(const Request&);
		Request&	operator=(const Request&);

		int		_parseSL();
		int		_parseHeader();
		int		_parseBody();
		bool	_wrong_method(const String&);
		bool	_wrong_version(const String&);

		Stream	_data;
		String	_body;
		Array	_start;
		Fields	_headers;
};

int	Request::_parseSL() {
	return SUCCESS;
}

int	Request::_parseHeader() {
	return SUCCESS;
}

int	Request::_parseBody() {
	return SUCCESS;
}

int	Request::parser() {
	int	err;

	err = 200;
	while (1) {

	}
	return err;
}

class	Response {
	public:

		~Response() {}
		Response(const Request& req) : _err(200) {}

	private:
		Response();
		Response(const Response&);
		Response&	operator=(const Response&);

		int		_err;
};

class	HttpContext {
	public:
		HttpContext(Parser* parser, Event::Pool& events, int& fd);
		~HttpContext();

		bool	loop(); // Event loop
		bool	format(); // format HTTP request
		bool	handleRequest(int socket); // handle request
		bool	handleResponse(int socket); // handle HTTP response
		int		newConnection(int socket) const; // handle new connection

		const Event&	getEvent() const
		{ return _multiplexing; }

	private:
		HttpContext();
		HttpContext(const HttpContext&);
		HttpContext&	operator=(const HttpContext&);

		String		_raw;
		Request*	_req;
		Response*	_res;
		Parser*		_parser;
		const Event	_multiplexing;
};
#endif