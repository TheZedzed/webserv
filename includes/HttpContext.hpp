#pragma once

# ifndef HTTPCONTEXT_HPP
# define HTTPCONTEXT_HPP

# include "Multiplexer.hpp"

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
		typedef Multiplexer::events_t	events_t;

		HttpContext(events_t& events, int& fd);
		~HttpContext() {}

		void	loop(); // Event loop
		bool	worker(int nfds); // manage events
		bool	handleRequest(); // handle received data
		int		newConnection(); // handle new connection

		const Multiplexer&	getMultiplexer() const;

	private:
		HttpContext();
		HttpContext(const HttpContext&);
		HttpContext&	operator=(const HttpContext&);

		bool	_addClient(int fd, const Client::servers_t& serv);
		bool	_modClient();
		bool	_delClient();
		
		Multiplexer	_multiplexer;
};

#endif