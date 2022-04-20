#include "Client.hpp"

Client::Client(int fd, const Event::Servers& serv) : _fd(fd), _servers(serv)
{}

Client::~Client() {
	std::cout << "Destroy client!" << std::endl;
	close(_fd);
	delete _request;
	delete _response;
}

void	Client::setReq(const String& raw)
{ _request = new Request(raw); }

void	Client::setRes(int code)
{ _response = new Response(*this, code); }

bool	Client::sendRes() const {
	const char	buffer[4096] = {0};
	int	wlen;

	wlen = send(_fd, buffer, sizeof(buffer) - 1, 0);
	if (wlen == -1)
		return FAILURE;
	return SUCCESS;
}