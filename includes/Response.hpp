#pragma once

# ifndef RESPONSE_HPP
# define RESPONSE_HPP

#include <dirent.h>
#include <ctime>
#include <ftw.h>

#include "Client.hpp"

/*
** class HTTP Response:
** build a simple HTTP response
** using an HTTP request
*/
class	Response {
	public:
		Response(str_t& raw);
		~Response();

		void	construct_route(const Location** loc, str_t& route) const;

		void	set_server(const Server* serv);
		void	set_request(const Request* req);
		void	error_response(int code);

		bool	extract_content(const str_t* path);
		void	set_header(int code, const str_t& path = str_t());
		bool	extract_directory(const str_t& route, const str_t& subroute);

	private:
		Response();
		Response(const Response&);
		Response&	operator=(const Response&);

		str_t&		_buffer;
		const Server*	_server;
		const Request*	_request;
};
#endif
