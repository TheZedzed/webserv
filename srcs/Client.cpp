#include "Client.hpp"

static bool	_method_allowed(const Location* uri_loc, const str_t& method) {
	strs_t::const_iterator	it;

	it = uri_loc->get_allowed().begin();
	for (; it != uri_loc->get_allowed().end(); ++it) {
		if (*it == method)
			return true;
	}
	return false;
}

static str_t	_fetch_mime(const str_t& route, int code) {
	fields_t::const_iterator	it;
	str_t	path;
	size_t	pos;

	if (code == 200) {
		path = route.substr(0, route.find_last_of('?'));
		pos = path.find_last_of('.');
		it = mime_g.find(path.substr(pos + 1));
		if (it != mime_g.end())
			return it->second;
	}
	return "text/html";
}

static bool _treat_cgi(const Response* res, int state) {
	static const char*	endings[3] = {".php", ".py", NULL};
	const Location*	loc = res->get_location();
	str_t	path = res->get_path();
	size_t	found;

	found = path.find_last_of('.');
	if (!(state & ERROR) && !loc->get_cgi().empty()) {
		if (found != std::string::npos) {
			for (int i = 0; i < 2; ++i) {
				if (path.substr(found) == endings[i])
					return true;
			}
		}
	}
	return false;
}

Client::Client() : _server(NULL), _request(NULL)
{ std::cout << "Create new client!" << std::endl; }

Client::~Client() {
	std::cout << "Destroy client!" << std::endl;
	delete _request;
}

void	Client::reset() {
	delete _request;
	raw_data.clear();
	_request = NULL;
	_server = NULL;
}

void	Client::_process_method(Response* res, int socket, int& state) {
	const Location*	loc = res->get_location();
	str_t	method = _request->get_rl()[0];

	std::cout <<  "REQUEST LINE:" << _request->get_rl()[0] << ' ' << _request->get_rl()[1] << ' ' <<_request->get_rl()[2] << "\n";
	if (_method_allowed(loc, method) == false) {
		state |= (ERROR | ERR_405);
		raw_data += res->error_response(state);
	}
	else if (_treat_cgi(res, state))
		res->process_cgi(socket, state, raw_data);
	else if (method == "GET")
		raw_data += res->process_get(state);
	else if (method == "DELETE")
		raw_data += res->process_delete(state);
	else {
		state |= (ERROR | ERR_501);
		raw_data += res->error_response(state);
	}
}

void	Client::set_headers(const str_t& path, int code) {
	str_t	data;
	str_t	time;

	std::time_t fetch_time = std::time(NULL);
	time = std::asctime(std::localtime(&fetch_time));
	*time.rbegin() = CR;
	data += "HTTP/1.1 " + _itoa(code) + " " + code_g[code] + CRLF;
	data += "Server: " SERVER CRLF;
	data += "Date: " + time;
	data += "\nContent-Type: " + _fetch_mime(path, code) + CRLF;
	data += "Content-Length: " + _itoa(raw_data.size()) + CRLF;
	if (code == 301)
		data += "Location: " + path + CRLF;
	if (code == 301 || code == 200 || code == 403 || code == 405)
		data += "Connection: Keep-alive" CRLF CRLF;
	else
		data += "Connection: close" CRLF CRLF;
	data += raw_data;
	raw_data = data;
}

void	Client::process_request(int& state) {
	if (_request == NULL)
		_request = new Request();
	if (state & RQLINE)
		state = _request->process_rl(raw_data);
	if (state & HEADER)
		state = _request->process_head(raw_data);
	if (state & BODY)
		state = _request->process_body(raw_data);
}

void	Client::process_response(int socket, int& state) {
	Response*	response;
	strs_t	rline;

	raw_data.clear();
	rline = _request->get_rl();
	response = new Response(get_server(), get_request());
	if (state & ERROR)
		raw_data += response->error_response(state);
	else if (_request->get_body().size() > _server->get_max()) {
		state |= (ERROR | ERR_413);
		raw_data += response->error_response(state);
	}
	else if (response->construct_path(state) == FAILURE)
		raw_data += response->error_response(state);
	else if (!(state & ERR_301))
		_process_method(response, socket, state);
	if (rline.size() == 3 && rline[2].substr(0, 5) == "HTTP/") {
		if (_treat_cgi(response, state) == false)
			set_headers(response->get_path(), _translate(state));
	}
	delete response;
	return ;
}
