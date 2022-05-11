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
		Response(const Server* server, const Request* request);
		~Response();

		int	get_code() const;
		const Server*	get_server() const;

		str_t&	code_response(const Location* uri_loc, int code) const;
		str_t&	process_method(const Location* uri_loc, const str_t& route) const;
		const Location*	construct_route(str_t& route) const;

	private:
		Response();
		Response(const Response&);
		Response&	operator=(const Response&);

		str_t&	_process_post(const Location* uri_loc) const;
		str_t&	_process_delete(const Location* uri_loc) const;
		str_t&	_process_get(const Location* uri_loc, bool is_dir) const;

		void	_set_header(str_t& buffer, int code) const; // header for error or success
		void	_set_header(str_t& buffer, const str_t& redir) const; // header for redir

		str_t&	_method_in_route(const Location* uri_loc) const;
		str_t&	_extract_page(int& code, const str_t* path) const;

		const Server*	_server;
		const Request*	_request;
};

#endif