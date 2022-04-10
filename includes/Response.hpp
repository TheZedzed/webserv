#pragma once

# ifndef RESPONSE_HPP
# define RESPONSE_HPP

#include "Request.hpp"

/*
** class HTTP Response:
** build a simple HTTP response
** using an HTTP request
*/
class	Response {
	public:
		Response(const Request& req);
		~Response();

	private:
		Response();
		Response(const Response&);
		Response&	operator=(const Response&);


};

#endif