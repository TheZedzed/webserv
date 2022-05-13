#include "Client.hpp"

Client::Client(const servers_t& serv) : _state(RQLINE), _servers(serv)
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
	servers_t::const_iterator	it1;
	strs_t::const_iterator		it2;
	str_t	match;

	it1 = _servers.begin();
	match = _request->get_headers().find("host")->second;
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
** TODO: format header in last
** process action
*/
void	Client::process_res() {
	const Location*	uri_loc;
	str_t	route;

	if (_state & ERROR) { // convert error during request time
		if (_state & ERR_400)
			_state = 400;
		else if (_state & ERR_404)
			_state = 404;
		else if (_state & ERR_505)
			_state = 505;
		else if (_state & ERR_501)
			_state = 501;
		raw_data += _response->code_response(NULL, _state);
	}
	else if (_request->get_body().size() > _response->get_server()->get_max())
		raw_data += _response->code_response(NULL, 413);
	else if ((uri_loc = _response->construct_route(route)) == NULL)
		raw_data += _response->code_response(uri_loc, 404);
	else if (route.size() > 256)
		raw_data += _response->code_response(uri_loc, 414);
	else
		raw_data += _response->process_method(uri_loc, route);
}
