#pragma once

# ifndef HTTPCONTEXT_HPP
# define HTTPCONTEXT_HPP

# include "Client.hpp"

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
		typedef std::map<int, Client*>	Clients;

		HttpContext(Event::Pool& events, int& fd);
		~HttpContext();

		void	loop(); // Event loop
		bool	format(); // format HTTP request
		bool	worker(int nfds); // manage events
		bool	handleRequest(); // handle received data
		bool	handleResponse(); // handle HTTP response
		int		newConnection(); // handle new connection

		const Event&	getMultiplexer() const;

	private:
		HttpContext();
		HttpContext(const HttpContext&);
		HttpContext&	operator=(const HttpContext&);

		bool	_addClient(int fd, const Event::Servers& serv);
		bool	_modClient(const String& raw);
		bool	_sendRes(int code);
		bool	_delClient(void);

		Clients		_clients;
		const Event	_multiplexer;
};

#endif