#include "HttpContext.hpp"

static const int	MAX_EVENTS = 256;
static const int	BUFFER_SIZE = 4096;
static epoll_event	events[MAX_EVENTS];

HttpContext::HttpContext(const Event::Pool& events, int fd) : _multiplexing(events, fd)
{ std::cout << "Creating webserver..." << std::endl;}

HttpContext::~HttpContext()
{ std::cout << "Destroy webserver" << std::endl; }

bool	HttpContext::handleResponse(int socket) {
	char	*buf;

	printf("request:\n%s\n", data_recv.c_str());
	sprintf(buf, "HTTP/1.1 200 OK\r\nContent-Length: %d\r\n\r\n Hello World!!!\n", 16);
	send(socket, buf, 55, 0);
	if (_multiplexing.delEvent(socket))
		return FAILURE;
	return SUCCESS;
}

bool	HttpContext::handleRequest(int socket) {
	String	buffer(BUFFER_SIZE, 0);
	int		rlen;

	while (26) {
		rlen = recv(socket, &buffer, BUFFER_SIZE - 1, 0);
		if (rlen == 0) {//peer disconnect
			if (_multiplexing.delEvent(socket))
				return FAILURE;
			break ;
		}
		else if (rlen == -1) {//finish read with non block fd (EAGAIN or EWOULDBLOCK)
			if (_multiplexing.modEvent(socket))
				return FAILURE;
			break ;
		}
		data_recv.append(buffer, rlen); // HTTP request
		buffer.clear();
	}
	return SUCCESS;
}

bool	HttpContext::loop(void) {
	int		epoll = _multiplexing.getInstance();
	int		res;
	int		nfds;
	int		peer_fd;

	if (_multiplexing.newEvent())
		return FAILURE;
	while (42) {
		if ((nfds = epoll_wait(epoll, events, MAX_EVENTS, 1000)) == -1)
			return FAILURE;
		for (int i = 0; i < nfds; ++i) {
			peer_fd = events[i].data.fd;
			if ((res = _multiplexing.newConnection(peer_fd)) == -1)
				return FAILURE;
			else if (res)
				continue ;
			if (events[i].events & EPOLLIN) {
				if (handleRequest(peer_fd))
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