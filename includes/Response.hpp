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
		Response(int code);
		~Response();

	private:
		Response();
		Response(const Response&);
		Response&	operator=(const Response&);

		const int		_code;
		const Client&	_client;
};

#endif