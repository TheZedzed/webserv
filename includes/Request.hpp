#pragma once

# ifndef REQUEST_HPP
# define REQUEST_HPP

# include "Base.hpp"

# include <cmath>

/*
** class HTTP Request:
** build and format a simple HTTP request
** extract request line in a vector of string
** extract header fields in a map
** extract body in a string
** decode chunked message
*/
class	Request {
	public:
		typedef std::map<String, String>	Fields;

		~Request();
		Request(const String& raw);

		void	format(const String& raw);

	private:
		Request();
		Request(const Request&);
		Request&	operator=(const Request&);

		void	_extractHead(size_t& pos, const String& raw);
		void	_extractChunk(size_t& pos, const String& raw);

		String	_body;
		Array	_start;
		Fields	_headers;
};

#endif