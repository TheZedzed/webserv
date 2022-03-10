#pragma once

# ifndef HTTPCONTEXT_HPP
# define HTTPCONTEXT_HPP

# include "Event.hpp"
# include "Parser.hpp"

class	Parser;

class	Response {
	public:
		typedef std::map<String, String>	Fields;
		typedef std::stringstream			Sstream;

		~Response() {
			_start.clear();
			_headers.clear();
		}
		Response(const String& req) : _raw(req), _err(200) {}

		void	parseSL();
		void	parseHead();
		void	parseBody();
		bool	response(int fd);

		const Fields&	getHead() const
		{ return _headers; }

	private:
		Response();
		Response(const Response&);
		Response&	operator=(const Response&);

		bool	_wrong_method(const String&);
		bool	_wrong_version(const String&);

		int		_err;
		Sstream	_raw;
		String	_body;
		Array	_start;
		Fields	_headers;
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

		Parser*		_parser;
		Response*	_handler;
		const Event	_multiplexing;
};
#endif