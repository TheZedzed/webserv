#include "Request.hpp"

static String&	_tolower(String& str) {
	size_t	i;

	i = 0;
	while (str[i]) {
		if (str[i] >= 65 || str[i] <= 90)
			str[i] += 32;
		++i;
	}
	return str;
}

static size_t	_atoi(const String& str, int b) {
	std::istringstream	tmp(str);
	size_t	res(0);

	if (b == 16)
		tmp >> std::hex >> res;
	else
		tmp >> std::dec >> res;
	return res;
}

static String	_itoa(int nb) {
	std::ostringstream	res;

	res << nb;
	return res.str();
}

static bool	_wrong_method(const String& elem)
{ return elem != "GET" && elem != "DELETE" && elem != "POST"; }

static bool	_wrong_version(const String& elem)
{ return elem != "HTTP/1.1"; }

Request::Request(const String& raw) {
	size_t	end;

	std::cout << "Reformat an http request" << std::endl;
	_extractSL(end, raw);
	_extractHead(end, raw);
	_extractBody(end, raw);
}

Request::~Request()
{ std::cout << "Destroy Request" << std::endl; }

const String&	Request::getBody() const
{ return _body; }

const Array&	Request::getSL() const
{ return _start; }

const Request::Fields&	Request::getHeaders() const
{ return _headers; }

void	Request::_extractBody(size_t& pos, const String& raw) {
	Fields::const_iterator	it1;
	Fields::const_iterator	it2;

	it1 = _headers.find("content-length:");
	it2 = _headers.find("transfert-encoding:");
	if (it2 != _headers.end()) {
		if (it2->second != "chunked")
			throw 501;
		_extractChunk(pos, raw);
	}
	else if (it1 != _headers.end())
		_body = raw.substr(pos);
	else
		throw 411;
}

void	Request::_extractSL(size_t& pos, const String& raw) {
	std::stringstream	tmp;
	String	elem;

	pos = raw.find("\r\n");
	if (pos != std::string::npos) {
		tmp.str(raw.substr(0, pos));
		while (tmp >> elem)
			_start.push_back(elem);
		pos += 2;
	}
	if (_start.size() != 3 || _wrong_method(_start[0]))
		throw 400;
	if (_wrong_version(_start[2]))
		throw 505;
}

void	Request::_extractHead(size_t& pos, const String& raw) {
	String	key;
	String	line;
	String	value;
	size_t	found1;
	size_t	found2;

	while ((found1 = raw.find("\r\n", pos)) != std::string::npos) {
		if (found1 != pos) {
			line = raw.substr(pos, found1);
			found2 = line.find(":");
			if (found2 == std::string::npos)
				throw 400;
			key = line.substr(0, found2);
			value = line.substr(found2 + 1, found1);
			_headers.insert(std::make_pair(_tolower(key), value));
		}
		pos = found1 + 2;
	}
}

void	Request::_extractChunk(size_t& pos, const String& raw) {
	size_t	chunk_size = 0;
	size_t	length = 0;
	size_t	found = 0;
	size_t	i = 0;

	pos += 2; // skip blank line after headers
	while (pos != std::string::npos) {
		found = raw.find("\r\n", pos);
		if (!(i % 2) && found != std::string::npos) {
			if (!(chunk_size = _atoi(raw.substr(pos, found - pos), 16)))
				break ;
			length += chunk_size;
			pos = found + 2;
		}
		else if ((i % 2) && found != std::string::npos) {
			_body.append(raw.substr(pos, found + 2));
			pos = found + 4;
		}
		++i;
	}
	_headers.erase("transfer-encoding:");
	_headers.insert(std::make_pair("content-length:", _itoa(length)));
}
