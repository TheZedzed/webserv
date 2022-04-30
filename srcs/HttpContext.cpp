#include "HttpContext.hpp"

static Connection*	peer;
static epoll_event	events[256];

HttpContext::HttpContext(Multiplexer::events_t& events, int& fd) : _multiplexer(events, fd)
{ std::cout << "Creating webserver!" << std::endl;}

const Multiplexer&	HttpContext::getMultiplexer() const
{ return _multiplexer; }

bool	HttpContext::handleRequest() {
	char	buffer[4096];
	Client*	client;
	int		rlen;

	bzero(buffer, 4096);
	client = peer->getClient();
	while ((rlen = recv(peer->getFd(), &buffer, 4095, 0)) != -1) {
		if (rlen == 0) {
			_delClient();
			return SUCCESS;
		}
		client->process_req(buffer); // todo
	}
	return SUCCESS;
}

bool	HttpContext::_modClient() {
	if (_multiplexer.modEvent(peer, EPOLLOUT))
		return FAILURE;
	return SUCCESS;
}

bool	HttpContext::_delClient() {
	if (_multiplexer.delEvent(peer))
		return FAILURE;
	return SUCCESS;
}

bool	HttpContext::_addClient(int fd, const Client::servers_t& servers) {
	HttpContext::events_t	events;
	Connection*	client;

	events = getMultiplexer().getEvents();
	client = new Connection(fd, CLIENT);
	events.insert(std::make_pair(fd, client));
	if (_multiplexer.addEvent(client, EPOLLIN | EPOLLET))
		return FAILURE;
	return SUCCESS;
}

int	HttpContext::newConnection() {
	struct sockaddr_in	client_addr;
	socklen_t	size;
	int			fd;

	size = sizeof(client_addr);
	bzero(&client_addr, size);
	if (peer->getType() == LISTENNER) {
		while (1) {
			fd = accept(peer->getFd(), (sockaddr *)&client_addr, &size);
			if (fd == -1)
				return FAILURE;
			if (_addClient(fd, peer->getServers()))
				return FAILURE;
			break ;
		}
		return SUCCESS;
	}
	return -1;
}

bool	HttpContext::worker(int nfds) {
	int	res;

	for (int i = 0; i < nfds; ++i) {
		peer = reinterpret_cast<Connection*>(events[i].data.ptr);
		res = newConnection();
		if (res == FAILURE)
			return FAILURE;
		else if (res == SUCCESS)
			continue ;
		if (events[i].events & EPOLLIN) {
			if (handleRequest())
				return FAILURE;
		}
		//if (events[i].events & EPOLLOUT) {
		//	if (handleResponse())
		//		return FAILURE;
		//}
	}
	return SUCCESS;
}

void	HttpContext::loop(void) {
	int	epoll;
	int	nfds;

	epoll = _multiplexer.getInstance();
	while (1) {
		try {
			nfds = epoll_wait(epoll, events, 256, 1000);
			if (nfds == -1)
				throw std::runtime_error("Failure epoll\n");
			if (worker(nfds))
				throw 500;
		}
		catch (int code) {
			// setflag peer RESPONSE
			// new response code
			// turn peer EPOLLOUT
			continue ;
		}
		catch (...) {
			// intern error
			throw ;
		}
	}
}