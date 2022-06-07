#pragma once

# ifndef HTTPCONTEXT_HPP
# define HTTPCONTEXT_HPP

# include "Multiplexer.hpp"
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
		typedef std::map<timer_t, Connection*>	timers_t;

		HttpContext(const char* conf_file);
		~HttpContext();

		void	worker(); // events loop
		bool	new_connection(); // manage new connection
		bool	handle_request(); // manage received data
		bool	handle_response(); // send data
		void	timeout(void* ptr); // client timeout

	private:
		HttpContext();
		HttpContext(const HttpContext&);
		HttpContext&	operator=(const HttpContext&);

		bool	_add_client(int socket);
		bool	_mod_client();
		bool	_del_client();

		timers_t		_timers;
		const Parser*	_parser;
		Multiplexer		_multiplexer;
};

#endif
