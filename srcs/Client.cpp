#include "Client.hpp"

Client::Client(const servers_t& serv) : _state(RQLINE), _request(), _response(), _servers(serv)
{ std::cout << "Create new client!" << std::endl; }

Client::~Client() {
	std::cout << "Destroy client!" << std::endl;
	delete _request;
	delete _response;
}

Request*	Client::get_request()
{ return _request; }

Response*	Client::get_response()
{ return _response; }

int	Client::get_state() const
{ return _state; }

void	Client::set_response(Response* response)
{ _response = response; }

void	Client::set_request(Request* request)
{ _request = request; }

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

bool	Client::_request_time_error() {
	str_t	body;
	size_t	max;
	bool	err;

	err = false;
	body = _request->get_body();
	max = _response->get_server()->get_max();
	if ((_state & ERROR) && (err = true))
		_response->error_response(_state & ~ERROR);
	else if (body.size() > max && (err = true))
		_response->error_response(413);
	return err;
}

bool	Client::_request_route_error(const Location* uri_loc, str_t& route) {
	struct stat	st;
	bool		err;
	int			res;

	err = true;
	uri_loc = _response->construct_route(route);
	res = stat(route.c_str(), &st);
	if (!uri_loc || res == -1) // to do: check specific error errno
		_response->error_response(404);
	else if (route.size() > 256)
		_response->error_response(414);
	else
		err = false;
	return err;
}

/*
** to do: format header in last
** process action
*/
void	Client::process_res() {
	const Location*	uri_loc;
	str_t	route;

	uri_loc = NULL;
	if (_request_time_error() == true) // error during request time
		return ;
	if (_request_route_error(uri_loc, route) == true) // error with URI
		return ;
	_response->process_method(uri_loc, route);	
}
