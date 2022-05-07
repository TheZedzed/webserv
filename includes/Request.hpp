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
*/
class	Request {
	public:
		typedef std::map<str_t, str_t>	fields_t;

		~Request();
		Request();

		bool	get_chunked() const;
		const str_t&	get_body() const;
		const strs_t&	get_rl() const;
		const fields_t&	get_headers() const;

		void	delete_header(str_t& key);
		void	extract_rl(const strs_t& raw);
		void	extract_body(const str_t& raw);
		void	extract_header(str_t& key, str_t& value);

		void	set_chunked(bool type);

	private:
		Request(const Request&);
		Request&	operator=(const Request&);

		str_t	_body;
		strs_t	_start;
		bool	_chunked;
		fields_t	_headers;
};

#endif