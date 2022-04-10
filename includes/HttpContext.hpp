#pragma once

# ifndef HTTPCONTEXT_HPP
# define HTTPCONTEXT_HPP

# include "Response.hpp"
# include "Parser.hpp"

/* class HttpContext:
** nginx like Http-context
** manage request/response following RFC 7230-7231
** listen on multiple servers
** multiplexing using epoll
** only works with simple HTTP request
** GET, POST, DELETE mothod implemented
** simple error managment
** chunked or simple HTTP message
** one or more cgi
** simple server config (see .conf files)
*/
class	HttpContext {
	public:
		HttpContext(Parser* parser, Event::Pool& events, int& fd);
		~HttpContext();

		bool	loop(); // Event loop
		bool	format(); // format HTTP request
		bool	handleRequest(int socket); // handle request
		bool	handleResponse(int socket); // handle HTTP response
		int		newConnection(int socket) const; // handle new connection

		const Event&	getMultiplexer() const;

	private:
		HttpContext();
		HttpContext(const HttpContext&);
		HttpContext&	operator=(const HttpContext&);

		int			_err_code;
		Response*	_response;
		Request*	_request;
		Parser*		_server_info;
		const Event	_multiplexer;
};

#endif