#include "Client.hpp"

static int	_translate(int hex) {
	if (hex == ERR_400)
		return 400;
	if (hex == ERR_403)
		return 403;
	if (hex == ERR_404)
		return 404;
	if (hex == ERR_405)
		return 405;
	if (hex == ERR_411)
		return 411;
	if (hex == ERR_413)
		return 413;
	if (hex == ERR_414)
		return 414;
	if (hex == ERR_500)
		return 500;
	if (hex == ERR_501)
		return 501;
	return 505;
}

Client::Client(const servers_t& serv) : _state(RQLINE), _request(NULL), _response(NULL), _servers(serv)
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

bool	Client::_request_time_error() {
	str_t	body;
	size_t	max;
	bool	err;

	err = false;
	body = _request->get_body();
	if (_response->get_server())
		max = _response->get_server()->get_max();
	else
		max = 4096;
	if ((_state & ERROR) && (err = true))
		_response->error_response(_translate(_state & ~ERROR));
	else if (body.size() > max && (err = true))
		_response->error_response(413);
	return err;
}

const Server*	Client::search_requested_domain() const {
	servers_t::const_iterator			it1;
	strs_t::const_iterator				it2;
	str_t								match;
	Request::fields_t::const_iterator	res;

	it1 = _servers.begin();
	res = _request->get_headers().find("host");
	if (res == _request->get_headers().end())
		return NULL;
	match = res->second;
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
	if (!_request)
		_request = new Request();
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
	struct stat	st;
	str_t	route;

	if (_request_time_error() == true)
		return ;
	uri_loc = _response->construct_route(route);
	if (stat(route.c_str(), &st) == -1) {
		if (errno == EACCES)
			return _response->error_response(403);
		if (errno == ENAMETOOLONG)
			return _response->error_response(414);
		if (errno == ENOENT)
			return _response->error_response(404);
		return _response->error_response(500);
	}
	if ((st.st_mode & S_IFMT) == S_IFDIR && *route.rbegin() != '/')
		route += "/";
	return _response->process_method(uri_loc, route);
}
