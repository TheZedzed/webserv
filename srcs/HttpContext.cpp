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

int	tcpSocket(const Socket& sock) {
	struct sockaddr_in	listen_address;
	int		socket_fd;
	int		res;

	res = 1;
	socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (socket_fd == -1)
		return -1;
	setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &res, sizeof(res));
	memset(&listen_address, 0, sizeof(sockaddr_in));
	listen_address.sin_family = AF_INET; //ipv4
	listen_address.sin_port = htons(std::atoi(sock.second.c_str()));
	listen_address.sin_addr.s_addr = htonl(inet_addr(sock.first.c_str()));
	res = bind(socket_fd, reinterpret_cast<sockaddr *>(&listen_address), sizeof(sockaddr_in));
	if (res == -1)
		return -1;
	res = listen(socket_fd, 100);//up to 100 connections
	if (res == -1)
		return FAILURE;
	return socket_fd;
}

bool	incorrect(const Array& line)
{ return true; }

bool	parse(Event::Pool* events, const String& file, int flag) {
	Server*		curr_serv;
	Config*		curr_conf;
	Location*	curr_loc;
	Array		tmp;

	//open file
	//if error
	//	return FAILURE
	while (1) { //not EOF
		//getline
		if (flag)
			curr_conf = new Config();
		//split whitespace in tmp
		if (tmp[0] == "server") {
			if (incorrect(tmp))
				return FAILURE;
			if (flag)
				curr_conf = new Config();
			while (tmp[0] != "}") {
				//getline
				//split whitespace in tmp
				if (tmp[0] == "listen") {
					if (incorrect(tmp))
						return FAILURE;
					else if (flag)
						curr_conf->setSocket(tmp);
				}
				else if (tmp[0] == "location") {
					String	key;

					if (incorrect(tmp))
						return FAILURE;
					else if (flag) {
						key = tmp[1];
						curr_loc = new Location();
					}
					while (tmp[0] != "}") {
						//getline
						//swith Location fields
						//	if (incorrect(tmp))
						//		return FAILURE;
						//	else if (flag)
						//		fill location field tmp[0] with tmp[1..n]
					}
					if (flag)
						curr_conf->setRoutes(key, &curr_loc);
				}
				else if (tmp[0] == "error_page") {
					if (incorrect(tmp))
						return FAILURE;
					else if (flag)
						curr_conf->setErrorPage(tmp);
				}
				else if (tmp[0] == "server_name") {
					if (incorrect(tmp))
						return FAILURE;
					else if (flag)
						curr_conf->setNames(tmp);
				}
				else if (tmp[0] == "max_client_body_size") {
					if (incorrect(tmp))
						return FAILURE;
					else if (flag)
						curr_conf->setMax(tmp[1]);
				}
				else
					return FAILURE;
			}
			if (flag) {
				std::pair<int, Server*>		item;
				int	socket;

				socket = tcpSocket(curr_conf->getSocket());
				curr_serv = new Server(curr_conf);
				item = std::make_pair(socket, curr_serv); 
				events->insert(item);
			}
		}
	}
	return SUCCESS;
}

bool	HttpContext::configure(const String& file) {
	Event::Pool	events;
	bool		error;

	error = parse(&events, file, 0); 
	if (error == false) {
		parse(&events, file, 1);
		_multiplexing = new Event(events);
	}
	return error;
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