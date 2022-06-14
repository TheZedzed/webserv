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
		Response(const Server* serv, const Request* req);
		~Response();

		const str_t&	get_path() const;
		const Location*	get_location() const;

		bool	construct_path(int& state);

		str_t	extract_content(int& state);
		str_t	extract_directory(int& state);

		void	process_cgi(int socket, int& state, str_t& raw);
		str_t	error_response(int& state);
		str_t	process_delete(int& state);
		str_t	process_redir(int& state);
		str_t	process_post(int& state);
		str_t	process_get(int& state);

	private:
		Response();
		Response(const Response&);
		Response&	operator=(const Response&);

		str_t	_path; // real url
		const Server*	_server; // requested server
		const Request*	_request; // request
		const Location*	_location; // location rules
};

const inline str_t&	Response::get_path() const
{ return _path; }

const inline Location*	Response::get_location() const
{ return _location; }

#endif
