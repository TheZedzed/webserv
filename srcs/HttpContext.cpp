#include "HttpContext.hpp"

static const int	MAX_EVENTS = 256;
static const int	BUFFER_SIZE = 4096;
static epoll_event	events[MAX_EVENTS];

HttpContext::HttpContext(Parser* parser, Event::Pool& events, int& fd) : _parser(parser), _multiplexing(events, fd)
{ std::cout << "Creating webserver..." << std::endl;}

HttpContext::~HttpContext() {
	std::cout << "Destroy webserver" << std::endl;
	delete _parser;
}

bool	HttpContext::handleResponse(int socket) {
	char	buffer[BUFFER_SIZE] = {0};

	printf("request:\n%s\n", data_recv.c_str());
	sprintf(buffer, "HTTP/1.1 200 OK\r\nContent-Length: %d\r\n\r\n Hello World!!!\n", 16);
	send(socket, buffer, 55, 0);
	if (_multiplexing.delEvent(socket))
		return FAILURE;
	return SUCCESS;
}

bool	HttpContext::handleRequest(int socket) {
	char	buffer[BUFFER_SIZE] = {0};
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
	}
	return SUCCESS;
}

int	HttpContext::newConnection(int socket) const {
	Event::Pool::const_iterator	listen_fd;
	struct sockaddr_in	client_addr;
	socklen_t	size;
	int			fd;

	size = sizeof(client_addr);
	bzero(&client_addr, size);
	listen_fd = _multiplexing.getEvents().find(socket);
	if (listen_fd != _multiplexing.getEvents().end()) {
		printf("New connection\n");
		while (42) { // loop cuz ET mode epoll
			fd = accept(listen_fd->first, reinterpret_cast<sockaddr *>(&client_addr), &size);
			if (fd <= 0)
				break ;
			if (_multiplexing.addEvent(fd, EPOLLIN | EPOLLET))
				return -1;
			else
				break ;
		}
		return 1;
	}
	return 0;
}

bool	HttpContext::loop(void) {
	int	peer_fd;
	int	nfds;
	int	res;

	while (42) {
		nfds = epoll_wait(_multiplexing.getInstance(), events, MAX_EVENTS, 1000);
		if (nfds == -1)
			return FAILURE;
		for (int i = 0; i < nfds; ++i) {
			peer_fd = events[i].data.fd;
			if ((res = newConnection(peer_fd)) == -1)
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