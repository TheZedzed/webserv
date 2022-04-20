#include "HttpContext.hpp"

static int			peer_fd;
static epoll_event	events[256];

HttpContext::HttpContext(Event::Pool& events, int& fd) : _multiplexer(events, fd)
{ std::cout << "Creating webserver!" << std::endl;}

HttpContext::~HttpContext() {
	Clients::iterator	it;

	std::cout << "Destroy webserver!" << std::endl;
	for (it = _clients.begin(); it != _clients.end(); ++it) {
		delete it->second;
	}
}

const Event&	HttpContext::getMultiplexer() const
{ return _multiplexer; }

bool	HttpContext::handleRequest() {
	char	buffer[4096] = {0};
	String	raw;
	int		rlen;

	while (26) {
		rlen = recv(peer_fd, &buffer, 4095, 0);
		if (rlen == 0) { //peer disconnect
			if (_delClient())
				return FAILURE;
			break ;
		}
		else if (rlen == -1) { //finish read with non block fd (EAGAIN or EWOULDBLOCK)
			if (_modClient(raw))
				return FAILURE;
			break ;
		}
		raw.append(buffer, rlen); // request recv
	}
	return SUCCESS;
}

/*
bool	HttpContext::handleResponse(int socket) {

	// search for servers listenning on the socket
	Event::Pool::const_iterator	servers;

	servers	= getMultiplexer().getEvents().find(socket);

	// looking for requested host
	Request::Fields::const_iterator	match;
	Event::Servers::const_iterator	serv;
	Array::const_iterator			curr;
	Array::const_iterator			end;
	bool	found;

	found = false;
	serv = (*servers).second.begin();
	match = _request->getHeaders().find("host:");
	//if (match == _request->getHeaders().end())
	//	throw 400;
	for ( ;serv != (*servers).second.end() && !found; ++serv) {
		end = (*serv)->getConfig()->getNames().end();
		curr = (*serv)->getConfig()->getNames().begin();
		for ( ;curr != end && match->second != *curr && !found; ++curr) {
			if (match->second == *curr)
				found = true;
		}
	}
	serv = !found ? (*servers).second.begin() : serv;

	// send response to the client
	size_t	offset = 0;
	size_t	size = _response->getData().size();
	const char	*str = _response->getData().c_str();

	while (1) {
		send(socket, str + offset, 4096, 0);
		size /= 4096;
		if (!size)
			break ;
		offset += 4096;
	}
	Response	success = Response(200, socket);
	delete _request;
	delete _response;
	return SUCCESS;
}
*/

bool	HttpContext::_modClient(const String& raw) {
	Clients::iterator	it;

	it = _clients.find(peer_fd);
	(it->second)->setReq(raw);
	if (_multiplexer.modEvent(peer_fd))
		return FAILURE;
	return SUCCESS;
}

bool	HttpContext::_delClient() {
	delete _clients[peer_fd];
	_clients.erase(peer_fd);
	if (_multiplexer.delEvent(peer_fd))
		return FAILURE;
	return SUCCESS;
}

bool	HttpContext::_addClient(int fd, const Event::Servers& servers) {
	Client*	client;

	client = new Client(fd, servers);
	_clients.insert(std::make_pair(fd, client));
	if (_multiplexer.addEvent(fd, EPOLLIN | EPOLLET))
		return FAILURE;
	return SUCCESS;
}

int	HttpContext::newConnection() {
	Event::Pool::const_iterator	listen;
	struct sockaddr_in	client_addr;
	socklen_t	size;
	int			fd;

	size = sizeof(client_addr);
	bzero(&client_addr, size);
	listen = _multiplexer.getEvents().find(peer_fd);
	if (listen != _multiplexer.getEvents().end()) {
		while (42) {
			fd = accept(listen->first, (sockaddr *)&client_addr, &size);
			if (fd == -1)
				return FAILURE;
			if (_addClient(fd, listen->second))
				return FAILURE;
			break ;
		}
		return SUCCESS;
	}
	return 2;
}

bool	HttpContext::worker(int nfds) {
	int	res;

	for (int i = 0; i < nfds; ++i) {
		peer_fd = events[i].data.fd;
		res = newConnection();
		if (res == FAILURE)
			return FAILURE;
		else if (res == SUCCESS)
			continue ;
		if (events[i].events & EPOLLIN) {
			if (handleRequest())
				return FAILURE;
		}
		if (events[i].events & EPOLLOUT) {
			if (handleResponse())
				return FAILURE;
		}
	}
	return SUCCESS;
}

bool	HttpContext::_sendRes(int code) {
	Clients::iterator	it;

	it = _clients.find(peer_fd);
	if (it != _clients.end()) {
		_clients[peer_fd]->setRes(code);
		if (_clients[peer_fd]->sendRes())
			return FAILURE;
		if (_delClient())
			return FAILURE;
	}
	return SUCCESS;
}

void	HttpContext::loop(void) {
	int	epoll;
	int	nfds;

	epoll = _multiplexer.getInstance();
	while (42) {
		try {
			nfds = epoll_wait(epoll, events, 256, 1000);
			if (nfds == -1)
				throw std::runtime_error("Failure epoll\n");
			if (worker(nfds))
				throw 500;
		}
		catch (int error) {
			if (_sendRes(error))
				throw ;
		}
		catch (...) {
			throw ;
		}
	}
}