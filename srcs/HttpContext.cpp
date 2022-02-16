#include "HttpContext.hpp"

static const int	MAX_EVENTS = 256;
static const int	BUFFER_SIZE = 4096;
static epoll_event	events[MAX_EVENTS];

HttpContext::HttpContext() {
	std::cout << "Creating webserver..." << std::endl;
}

HttpContext::~HttpContext() {
	std::cout << "Destroy webserver" << std::endl;
	this->destroy();
}

HttpContext&	HttpContext::operator=(const HttpContext& rgh) {

}

bool	HttpContext::configure(const std::string& file) {
	std::vector<Server*>	servers;
	bool	error;

	/*
	** parse config here
	** push back in servers
	** construct ultiplexing with servers
	** while not EOF
	** LOOP:
	**	|create server
	**	|push_back sockets
	**	|..
	**	|create location
	*/
	if (servers.empty() || error)
		return FAILURE;
	this->_multiplexing = new Event(servers);
	return SUCCESS;
}

bool	HttpContext::destroy(void)
{ delete _multiplexing; }

bool	HttpContext::handleResponse(int socket) {
	char	*buf;

	sprintf(buf, "HTTP/1.1 200 OK\r\nContent-Length: %d\r\n\r\n Hello World!!!\n", 16);
	send(socket, buf, 55, 0);
	if (_multiplexing->delEvent(socket))
		return FAILURE;
	return SUCCESS;
}

bool	HttpContext::handleRequest(epoll_event event, int socket) {
	std::string	buffer(BUFFER_SIZE, 0);
	int		rlen;

	while (26) {
		rlen = recv(socket, &buffer, BUFFER_SIZE - 1, 0);
		if (rlen == 0) {//peer disconnect
			if (_multiplexing->delEvent(socket))
				return FAILURE;
			break ;
		}
		else if (rlen == -1) {//finish read with non block fd (EAGAIN or EWOULDBLOCK)
			if (_multiplexing->modEvent(event, socket))
				return FAILURE;
			break ;
		}
		data_recv.append(buffer, rlen);
		buffer.clear();
	}
	data_recv.clear();
	return SUCCESS;
}

bool	HttpContext::loop(void) {
	int		epoll = _multiplexing->getEpoll();
	int		res;
	int		nfds;
	int		peer_fd;

	if (_multiplexing->newEvent())
		return FAILURE;
	while (42) {
		if ((nfds = epoll_wait(epoll, events, MAX_EVENTS, 1000)) == -1)
			return FAILURE;
		for (int i = 0; i < nfds; ++i) {
			peer_fd = events[i].data.fd;
			if ((res = _multiplexing->newConnection(peer_fd)) == -1)
				return FAILURE;
			else if (res)
				continue ;
			if (events[i].events & EPOLLIN) {
				if (handleRequest(events[i], peer_fd))
					return FAILURE;
			}
			if (events[i].events & EPOLLOUT) {
				if(handleResponse(peer_fd))
					return FAILURE;
			}
		}
	}
	return SUCCESS;
}