#include "Request.hpp"

static str_t&	_tolower(str_t& str) {
	size_t	i = 0;
	while (str[i]) {
		if (str[i] >= 65 || str[i] <= 90)
			str[i] += 32;
		++i;
	}
	return str;
}

static str_t&	_clean(str_t& str) {
	while (str[0] == ' ')
		str.erase(0, 1);
	return str;
}

Request::Request() : _chunked(false)
{ std::cout << "Create an http request" << std::endl; }

Request::~Request()
{ std::cout << "Destroy Request" << std::endl; }

bool	Request::get_chunked() const
{ return _chunked; }

const str_t&	Request::get_body() const
{ return _body; }

const strs_t&	Request::get_rl() const
{ return _start; }

const Request::fields_t&	Request::get_headers() const
{ return _headers; }

void	Request::extract_rl(const strs_t& raw)
{ _start = raw; }

void	Request::extract_header(str_t& key, str_t& value)
{ _headers.insert(std::make_pair(_tolower(key), _clean(value))); }

void	Request::delete_header(str_t& key)
{ _headers.erase(key); }

void	Request::extract_body(const str_t& raw)
{ _body.append(raw); }

void	Request::set_chunked(bool type)
{ _chunked = type; }
