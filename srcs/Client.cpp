#include "Client.hpp"

static int	_translate(int hex) {
	if (hex == ERR_400)
		return 400;
	if (hex == ERR_403)
		return 403;
	if (hex == ERR_404)
		return 404;
	if (hex == ERR_414)
		return 414;
	if (hex == ERR_500)
		return 500;
	if (hex == ERR_501)
		return 501;
	return 505;
}

Client::Client(const servers_t& serv) : _state(RQLINE), _request(new Request()), _response(NULL), _servers(serv)
{ std::cout << "Create new client!" << std::endl; }

Client::~Client() {
	std::cout << "Destroy client!" << std::endl;
	delete _request;
	delete _response;
}

void	Client::clear() {
	delete _response;
	delete _request;
	raw_data.clear();
	set_request(new Request());
	set_response(NULL);
	set_state(RQLINE);
}

Request*	Client::get_request()
{ return _request; }

Response*	Client::get_response()
{ return _response; }

int	Client::get_state() const
{ return _state; }

void	Client::set_state(int state)
{ _state = state; }

void	Client::set_response(Response* response)
{ _response = response; }

void	Client::set_request(Request* request)
{ _request = request; }

bool	Client::_route_error(str_t& route) {
	struct stat	st;

	if (stat(route.c_str(), &st) == -1) {
		if (errno == EACCES)
			set_state(ERROR | ERR_403);
		else if (errno == ENOENT)
			set_state(ERROR | ERR_404);
		else if (errno == ENAMETOOLONG)
			set_state(ERROR | ERR_414);
		else
			set_state(ERROR | ERR_500);
		return true;
	}
	if ((st.st_mode & S_IFMT) == S_IFDIR && *route.rbegin() != '/')
		route += "/";
	return false;
}

const Server*	Client::search_requested_domain() const {
	servers_t::const_iterator			it1;
	strs_t::const_iterator				it2;
	str_t								match;
	Request::fields_t::const_iterator	res;

	it1 = _servers.begin();
	res = _request->get_headers().find("host");
	match = res->second.substr(0, res->second.find_last_of(':')); // host: [name:port]
	for (; it1 != _servers.end(); ++it1) {
		it2 = (*it1)->get_names().begin();
		for (; it2 != (*it1)->get_names().end(); ++it2) {
			if (*it2 == match)
				return *it1;
		}
	}
	return _servers[0];
}

void	Client::process_req(const str_t& raw_received) {
	raw_data.append(raw_received);
	if (_state & RQLINE)
		_state = _request->process_rl(raw_data);
	if (_state & HEADER)
		_state = _request->process_head(raw_data);
	if (_state & BODY)
		_state = _request->process_body(raw_data);
}

/*
** to do: describe function
*/
void	Client::process_res() {
	const Location*	uri_loc;
	str_t	route;
	size_t	max;

	max = _response->get_server() ? _response->get_server()->get_max() : 4096;
	if ((_state & ERROR))
		return _response->error_response(_translate(_state & ~ERROR));
	else if (_request->get_body().size() > max)
		return _response->error_response(413);
	uri_loc = _response->construct_route(route);
	if (_route_error(route) == true)
		return _response->error_response(_translate(_state & ~ERROR));
	return _response->process_method(uri_loc, route);
}
