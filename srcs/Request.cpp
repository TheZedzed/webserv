#include "Request.hpp"

static String&	_tolower(String str) {
	size_t	i;

	i = 0;
	while (str[i]) {
		if (str[i] >= 65 || str[i] <= 90)
			str[i] += 32;
		++i;
	}
	return str;
}

static String&	_itoa(int nb) {
	String	res;

	do {
		res.insert(res.begin(), (nb % 10) + '0');
		nb /= 10;
	} while (nb);
	return res;
}

static size_t	_toint(const String& str) {
	const static char*	hex = "0123456789abcdef";
	String::reverse_iterator	it;
	String	tmp;
	size_t	pow;
	size_t	nb;

	pow = nb = 0;
	tmp = _tolower(str);
	for (it = str.rbegin(); it != str.rend(); ++it) {
		nb += (hex[*it - '0'] * std::pow(16, pow));
		++pow;
	}
	return nb;
}

Request::Request(const String& raw) {
	std::cout << "Create an http request" << std::endl;
	format(raw);
}

Request::~Request()
{ std::cout << "Destroy Request" << std::endl; }

void	Request::format(const String& raw) {
	std::stringstream	tmp;
	String	elem;
	size_t	end;

	end = raw.find("\r\n");
	if (end != std::string::npos) {
		tmp.str(raw.substr(0, end));
		while (tmp >> elem)
			_start.push_back(elem);
		end += 2; // skip SL "\r\n"
		_extractHead(end, raw);
		end == std::string::npos ? 0 : end += 2; // skip empty line after headers
		if (_headers.find("content-length:") != _headers.end())
			_body = raw.substr(end);
		else if (_headers.find("transfert-encoding:") != _headers.end()) {
			if (_headers.find("transfert-encoding:")->second == "chunked")
				_extractChunk(end, raw);
		}
	}
}

void	Request::_extractHead(size_t& pos, const String& raw) {
	String	key;
	String	line;
	String	value;
	size_t	found1;
	size_t	found2;

	while (1) {
		found1 = raw.find("\r\n", pos);
		if (found1 != std::string::npos && found1 != pos) { // not empty line or EOF
			line = raw.substr(pos, found1);
			found2 = line.find(":");
			if (found2 != std::string::npos) {
				key = line.substr(0, found2);
				value = line.substr(found2 + 1, found1);
				_headers.insert(std::make_pair(_tolower(key), value));
			}
		}
		else
			break ;
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
			if (!(chunk_size = _toint(raw.substr(pos, found - pos))))
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
