#include "Request.hpp"

static str_t&	_tolower(str_t& str) {
	size_t	i;

	i = 0;
	while (str[i]) {
		if (str[i] >= 65 || str[i] <= 90)
			str[i] += 32;
		++i;
	}
	return str;
}

static size_t	_atoi(const str_t& str, int b) {
	std::ifstream	tmp(str);
	size_t	res(0);

	if (b == 16)
		tmp >> std::hex >> res;
	else
		tmp >> std::dec >> res;
	return res;
}

static str_t	_itoa(int nb) {
	std::stringstream	res;

	res << nb;
	return res.str();
}

static bool	_wrong_method(const str_t& elem)
{ return elem != "GET" && elem != "DELETE" && elem != "POST"; }

static bool	_wrong_version(const str_t& elem)
{ return elem != "HTTP/1.1"; }

Request::Request()
{ std::cout << "Create an http request" << std::endl; }

Request::~Request()
{ std::cout << "Destroy Request" << std::endl; }

const str_t&	Request::getBody() const
{ return _body; }

const strs_t&	Request::getSL() const
{ return _start; }

const Request::Fields&	Request::getHeaders() const
{ return _headers; }

void	Request::extractBody(const str_t& raw) {
	Fields::const_iterator	it1;
	Fields::const_iterator	it2;

	pos += 2;
	it1 = _headers.find("content-length:");
	it2 = _headers.find("transfert-encoding:");
	if (it2 != _headers.end()) {
		if (it2->second != "chunked")
			throw 501;
		_extractChunk(pos, raw);
	}
	else if (it1 != _headers.end())
		_body = raw.substr(pos);
	else if (it1 == _headers.end() && raw[pos])
		throw 411;
}

void	Request::extractSL(const str_t& raw) {
	std::stringstream	tmp;
	str_t	elem;
	size_t	pos;

	while (tmp >> elem)
		_start.push_back(elem);
	pos += 2;
	if (_start.size() != 3 || _wrong_method(_start[0]))
		throw 400;
	if (_wrong_version(_start[2]))
		throw 505;
	if (_start[1].size() + 7 > 100)
		throw 414;
}

void	Request::extractHeaders(const str_t& raw) {
	str_t	key;
	str_t	line;
	str_t	value;
	size_t	found1;
	size_t	found2;

	while (1) {
		found1 = raw.find("\r\n", pos);
		if (found1 == str_t::npos)
			break ;
		else if (found1 == pos) {
			extractBody(raw);
			break ;
		}
		else if (found1 != pos) {
			line = raw.substr(pos, found1);
			found2 = line.find(":");
			if (found2 == str_t::npos)
				throw 400;
			key = line.substr(0, found2);
			value = line.substr(found2 + 1, found1);
			_headers.insert(std::make_pair(_tolower(key), value));
		}
		pos = found1 + 2;
	}
}

void	Request::_extractChunk(const str_t& raw) {
	size_t	chunk_size = 0;
	size_t	length = 0;
	size_t	found = 0;
	size_t	i = 0;

	pos += 2; // skip blank line after headers
	while (pos != str_t::npos) {
		found = raw.find("\r\n", pos);
		if (!(i % 2) && found != str_t::npos) {
			if (!(chunk_size = _atoi(raw.substr(pos, found - pos), 16)))
				break ;
			length += chunk_size;
			pos = found + 2;
		}
		else if ((i % 2) && found != str_t::npos) {
			_body.append(raw.substr(pos, found + 2));
			pos = found + 4;
		}
		++i;
	}
	_headers.erase("transfer-encoding:");
	_headers.insert(std::make_pair("content-length:", _itoa(length)));
}
