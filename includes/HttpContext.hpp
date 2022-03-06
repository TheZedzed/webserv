#pragma once

# ifndef HTTPCONTEXT_HPP
# define HTTPCONTEXT_HPP

# include "Event.hpp"
# include "Parser.hpp"

class	Parser;

struct	Http {
	Array	start_line;
	std::map<String, String>	headers;
	String	body;
};

class	HttpContext {
	public:
		//typedef std::vector<Cgi>	Cgis;

		HttpContext(Parser* parser, Event::Pool& events, int& fd);
		~HttpContext();

		bool	loop(); // Event loop
		bool	handleRequest(int socket); // handle HTTP request
		bool	handleResponse(int socket); // handle HTTP response
		int		newConnection(int socket) const; // handle new connection

	private:
		HttpContext();
		HttpContext(const HttpContext&);
		HttpContext&	operator=(const HttpContext&);

		Parser*		_parser;
		Http		_handler;
		const Event	_multiplexing;
};
#endif