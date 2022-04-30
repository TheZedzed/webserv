#include "Client.hpp"

Client::Client(const servers_t& serv) : _flags(RQLINE), _servers(serv)
{ std::cout << "Create new client!" << std::endl; }

Client::~Client() {
	std::cout << "Destroy client!" << std::endl;
	delete _request;
	delete _response;
}

void	Client::setFlag(int bit)
{ _flags = bit; }

Request*	Client::getRequest() const
{ return _request; }

Response*	Client::getResponse() const
{ return _response; }

void	Client::_process_sl(const str_t& raw) {
	size_t	found;

	if (!raw.empty()) {
		found = raw.find("\r\n");
		if (found == std::string::npos) // still receive start line
			_request->extractSL(raw);
		else
			setFlag(HEADER); // good start line, process headers
	}
}

void	Client::_process_head(const str_t& raw)
{ _request->extractHeaders(raw); }

void	Client::_process_body(const str_t& raw)
{ _request->extractBody(raw); }

void	Client::process_req(const str_t& raw) {
	if (!_request)
		_request = new Request();
	if (_flags & RQLINE)
		_process_sl(raw);
	else if (_flags & HEADER)
		_process_head(raw);
	else if (_flags & BODY)
		_process_body(raw);
	else if (_flags = RESPONSE)
		_process_res();
}
