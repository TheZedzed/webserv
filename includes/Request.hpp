#pragma once

# ifndef REQUEST_HPP
# define REQUEST_HPP

# include "Base.hpp"

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
		typedef std::map<str_t, str_t>	Fields;

		~Request();
		Request();

		const str_t&	getBody() const;
		const strs_t&	getSL() const;
		const Fields&	getHeaders() const;

		void	extractSL(const str_t& raw);
		void	extractBody(const str_t& raw);
		void	extractHeaders(const str_t& raw);

	private:
		Request(const Request&);
		Request&	operator=(const Request&);

		void	_extractChunk(const str_t& raw);

		str_t	_body;
		strs_t	_start;
		Fields	_headers;
};

#endif