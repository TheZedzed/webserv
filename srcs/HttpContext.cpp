#include "HttpContext.hpp"

static const int	MAX_EVENTS = 256;
static const int	BUFFER_SIZE = 4096;
static epoll_event	events[MAX_EVENTS];

HttpContext::HttpContext()
{ std::cout << "Creating webserver..." << std::endl; }

HttpContext::~HttpContext() {
	std::cout << "Destroy webserver" << std::endl;
	delete _multiplexing;
}

/*
static bool	createSocket(std::map<int, Server*>* events, Server* server, const String& address, const String& port) {
	struct sockaddr_in	listen_address;
	int		socket_fd;
	int		res;

	res = 1;
	socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (socket_fd == -1)
		return FAILURE;
	setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &res, sizeof(res));
	memset(&listen_address, 0, sizeof(sockaddr_in));
	listen_address.sin_family = AF_INET; //ipv4
	listen_address.sin_port = htons(std::atoi(port.c_str()));
	listen_address.sin_addr.s_addr = htonl(inet_addr(address.c_str()));
	res = bind(socket_fd, reinterpret_cast<sockaddr *>(&listen_address), sizeof(sockaddr_in));
	if (res == -1)
		return FAILURE;
	res = listen(socket_fd, 100);//up to 100 connections
	if (res == -1)
		return FAILURE;
	events->insert(std::pair<int, Server*>(socket_fd, server));
	return SUCCESS;
}
*/

bool	HttpContext::configure(const String& file) {
	std::map<int, Server*>	events;
	Server*	curr_serv;
	Config*	curr_conf;
	bool	error;

	/*
	** parse config here
	** Read file while not EOF
	** LOOP:
	**	|create server
	**	|create current config (fill it)
	**	| LOOP:
	**		createSocket(&events, curr_serv, "address", "port");
	*/
	if (events.empty() || error)
		return FAILURE;
	_multiplexing = new Event(events);
	return SUCCESS;
}

bool	HttpContext::handleResponse(int socket) {
	char	*buf;

	sprintf(buf, "HTTP/1.1 200 OK\r\nContent-Length: %d\r\n\r\n Hello World!!!\n", 16);
	send(socket, buf, 55, 0);
	if (_multiplexing->delEvent(socket))
		return FAILURE;
	return SUCCESS;
}

bool	HttpContext::handleRequest(int socket) {
	String	buffer(BUFFER_SIZE, 0);
	int		rlen;

	while (26) {
		rlen = recv(socket, &buffer, BUFFER_SIZE - 1, 0);
		if (rlen == 0) {//peer disconnect
			if (_multiplexing->delEvent(socket))
				return FAILURE;
			break ;
		}
		else if (rlen == -1) {//finish read with non block fd (EAGAIN or EWOULDBLOCK)
			if (_multiplexing->modEvent(socket))
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
	int		epoll = _multiplexing->getInstance();
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