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

		void	worker(); // events loop
		void	manage_event(int id); // manage event
		bool	new_connection(); // manage new connection
		bool	handle_request(); // manage received data
		bool	handle_response(); // send data

		const Multiplexer&	getMultiplexer() const;

	private:
		HttpContext();
		HttpContext(const HttpContext&);
		HttpContext&	operator=(const HttpContext&);

		bool	_add_client(int socket);
		bool	_mod_client();
		bool	_del_client();

		Multiplexer	_multiplexer;
};

#endif
