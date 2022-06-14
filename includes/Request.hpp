#pragma once

# ifndef REQUEST_HPP
# define REQUEST_HPP

# include "Parser.hpp"

/*
** class HTTP Request:
** build and format a simple HTTP request
** extract request line in a vector of string
** extract header fields in a map
** extract body in a string
*/
class	Request {
	public:
		~Request();
		Request();

		const strs_t&	get_rl() const;
		const str_t&	get_body() const;
		const fields_t&	get_headers() const;

		int	process_rl(str_t& raw_data);
		int	process_head(str_t& raw_data);
		int	process_body(str_t& raw_data);
		int	process_chunk(str_t& raw_data);

	private:
		Request(const Request&);
		Request&	operator=(const Request&);

		int	_rline_checker();
		int	_headers_checker();

		str_t	_body;
		strs_t	_start;
		fields_t	_headers;
};

const inline str_t&		Request::get_body() const
{ return _body; }

const inline strs_t&	Request::get_rl() const
{ return _start; }

const inline fields_t&	Request::get_headers() const
{ return _headers; }

#endif