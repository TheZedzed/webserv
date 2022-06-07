#pragma once

# ifndef RESPONSE_HPP
# define RESPONSE_HPP

#include "Client.hpp"

/*
** class HTTP Response:
** build a simple HTTP response
** using an HTTP request
*/
class	Response {
	public:
		Response(const Server* server, const Request* request, str_t& raw);
		~Response();

		int	get_code() const;
		const Server*	get_server() const;

		const Location*	construct_route(str_t& route) const;

		void	error_response(int code);
		void	process_method(const Location* uri_loc, const str_t& route);
		void	process_post(const Location* uri_loc, const str_t& route);
		void	process_delete(const Location* uri_loc, const str_t& route);
		void	process_get(const Location* uri_loc, const str_t& route);

	private:
		Response();
		Response(const Response&);
		Response&	operator=(const Response&);

		str_t	_fetch_mime(int code);
		void	_set_header(int code, const str_t* redir = NULL);
		bool	_method_allowed(const Location* uri_loc, const str_t& method) const;
		bool	_extract_content(const str_t* path);
		bool	_extract_directory(const str_t& route, const str_t& subroute);

		str_t&			_buffer;
		const Server*	_server;
		const Request*	_request;
};

#endif
