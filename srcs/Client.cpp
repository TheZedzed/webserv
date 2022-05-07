#include "Client.hpp"

static size_t	_atoi(const str_t& str, int b) {
	std::ifstream	tmp(str);
	size_t	res(0);

	(b == 16) ? tmp >> std::hex >> res : tmp >> std::dec >> res;
	return res;
}

static str_t	_itoa(int nb) {
	std::stringstream	res;

	res << nb;
	return res.str();
}

static bool	_bad_method(const str_t& str) {
	str_t::const_iterator	it;

	for (it = str.begin(); it != str.end(); ++it) {
		if (!isupper(*it))
			return true;
	}
	return false;
}

static bool	_bad_uri(const str_t& str)
{ return str[0] != '/'; }

static int	_bad_version(const strs_t& str) {
	str_t	tmp;
	int	res;

	res = 0;
	tmp = str[2].substr(0, 4);
	if (tmp != "HTTP/") {
		if (str[0] != "GET" || str[0] != "POST" || str[0] != "DELETE")
			res = 40;
		else
			res = 44;
	}
	else {
		tmp = str[2].substr(5);
		if (tmp != "1.1")
			res = 505;
	}
	return res;
}

static void	_rline_error(strs_t& line) {
	size_t	size;
	int		err;

	size = line.size();
	if (size != 3 || _bad_method(line[0]) || _bad_uri(line[1]))
		err = 400;
	else
		err = _bad_version(line);
	if (err)
		(err == 40 || err == 44) ? throw err % 10 : throw err;
}

static void	_hline_error(Request* request) {
	Request::fields_t	headers;

	headers = request->get_headers();
	if (headers.find("host") == headers.end())
		throw 400;
	else if (headers.find("transfer-encoding") != headers.end()) {
		if (headers.find("transfer-encoding")->second != "chunked")
			throw 501;
		request->set_chunked(true);
	}
	else if (headers.find("content-length") != headers.end())
		request->set_chunked(false);
	else
		throw 200;
}

static void	_decode_chunk(Request* request, str_t& raw, size_t limit) {
	static bool		read_size = true;
	static size_t	length = 0;
	size_t chunk_size;

	while (!raw.empty()) {
		limit = raw.find(CRLF);
		if (limit != std::string::npos) {
			if (read_size == true) {
				if (!(chunk_size = _atoi(raw.substr(0, limit), 16)))
					throw 200;
				raw.erase(0, limit + 2);
				length += chunk_size;
				read_size = false;
			}
			else {
				request->extract_body(raw.substr(0, limit + 2));
				raw.erase(0, limit + 2);
			}
		}
	}
}

Client::Client(const servers_t& serv) : _flags(RQLINE), _servers(serv)
{ std::cout << "Create new client!" << std::endl; }

Client::~Client() {
	std::cout << "Destroy client!" << std::endl;
	delete _request;
	delete _response;
}

void	Client::set_flag(int bit)
{ _flags = bit; }

int		Client::get_flags() const
{ return _flags; }

Request*	Client::get_request()
{ return _request; }

Response*	Client::get_response()
{ return _response; }

/*
Aim: extract request line
skip first empty lines
perform error check when CRLF reached
resize peer buffer data
save the peer request line or throw an error
*/
void	Client::_process_sl(str_t& raw) {
	std::stringstream	tmp;
	size_t	limit;
	strs_t	line;
	str_t	elem;

	while (raw.substr(0, 2) == CRLF)
		raw.erase(0, 2);
	limit = raw.find(CRLF);
	if (limit != std::string::npos) { // complete request line
		tmp.str(raw.substr(0, limit));
		raw.erase(0, limit + 2);
		while (tmp >> elem)
			line.push_back(elem);
		_rline_error(line);
		_request->extract_rl(line);
		set_flag(HEADER);
	}
}

void	Client::_process_head(str_t& raw) {
	str_t	hline, value, key;
	size_t	limit, found;

	while (!raw.empty()) {
		limit = raw.find(CRLF);
		if (limit == 0) { // end of line headers
			_hline_error(_request);
			set_flag(BODY);
			break ;
		}
		else if (limit != std::string::npos) {
			hline = raw.substr(0, limit);
			found = hline.find(":");
			if (found != std::string::npos) { // good header line
				key = hline.substr(0, found - 1); 
				value = hline.substr(found + 1);
				_request->extract_header(key, value);
			}
		}
		else
			break ;
		raw.erase(0, limit + 2);
	}
}

void	Client::_process_body(str_t& raw) {
	Request::fields_t::const_iterator	it;
	Request*	request;
	size_t	limit;
	size_t	len;

	request = get_request();
	if (!request->get_chunked()) {
		it = request->get_headers().find("content-length");
		len = _atoi(it->second, 10);
		if (request->get_body().size() < len) {
			_request->extract_body(raw);
			raw.clear();
		}
	}
	else
		_decode_chunk(request, raw, limit);
}

void	Client::process_req(str_t& raw) {
	if (!_request)
		_request = new Request();
	if (_flags & RQLINE)
		_process_sl(raw);
	if (_flags & HEADER)
		_process_head(raw);
	if (_flags & BODY)
		_process_body(raw);
}
