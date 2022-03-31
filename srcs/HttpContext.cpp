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

bool	Response::_wrong_method(const String& elem)
{ return elem != "GET" && elem != "DELETE" && elem != "POST"; }

bool	Response::_wrong_version(const String& elem)
{ return elem != "HTTP/1.1"; }

/*
void	Response::parseSL() {
	std::stringstream	tmp;
	String	line;
	String	elem;

	std::getline(_raw, line);
	tmp.str(line);
	while (tmp >> elem)
		_start.push_back(elem);
	if (_start.size() != 3 || _wrong_method(_start[0]))
		_err = 400;
	else if (_wrong_version(_start[2]))
		_err = 505;
}

void	Response::parseHead() {
	Sstream	tmp;
	String	line;
	String	key;
	String	value;

	if (_err == 200) {
		while (std::getline(_raw, line) && line.size()) {
			tmp.clear();
			tmp.str(line);
			tmp >> key;
			tmp >> value;
			_headers.insert(std::make_pair(key, value));
		}
	}
}

bool	HttpContext::handleResponse(int socket) {

	_handler = new Response(data_recv);// create a dynamic response with data_recv
	_handler->parseSL();// parse start line
	_handler->parseHead();// parse headers
	_handler->parseBody();// parse body if any

	// search for servers listenning on the socket (map<socket, Servers*>)
	Event::Pool::const_iterator	it = getEvent().getEvents().find(socket);

	if (it != getEvent().getEvents().end()) {
		Event::Servers::const_iterator	it2 = (*it).second.begin();

		//search for host in servers
		for (; it2 != (*it).second.end(); ++it2) { // loop on servers
			Response::Fields::const_iterator	res = _handler->getHead().find("Host"); // cherche le host (de data_recv) correspondant  dans le vecteur de serverur 

			//loop on it2 (curr server names)
			Array::const_iterator	it3 = (*it2)->getConfig()->getNames().begin();
			for (; it3 != (*it2)->getConfig()->getNames().end(); ++it3) {
				if (res->second == *it3)
					"make response with it2 Server info"
			}
		}
		"make response with FIRST SERVER info ON VECTOR"

		Config::Routes::const_iterator	it3 = (*it2)->getConfig()->getRoutes().find("/"); // cherche la route dans it2 server
		if ((*it3).second->getCgi() != "")
			execute CGI;

		if (_handler.getBody().size() >= (*it2)->getConfig()->getMax())
			return (error("TOO LONG BODY"););
	}


	//send(socket, buffer, 55, 0);
	delete _handler;
	if (_multiplexing.delEvent(socket))
		return FAILURE;
	return SUCCESS;
}
*/

bool	HttpContext::handleResponse(int socket) {
	(void)socket;
	return SUCCESS;
}

bool	HttpContext::handleRequest(int socket) {
	char	buffer[BUFFER_SIZE] = {0};
	int		rlen;

	while (26) {
		rlen = recv(socket, &buffer, BUFFER_SIZE - 1, 0);
		if (rlen == 0) { //peer disconnect
			if (_multiplexing.delEvent(socket))
				return FAILURE;
			break ;
		}
		else if (rlen == -1) { //finish read with non block fd (EAGAIN or EWOULDBLOCK)
			if (_multiplexing.modEvent(socket))
				return FAILURE;
			break ;
		}
		_raw.append(buffer, rlen); // request recv
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
			fd = accept(listen_fd->first, (sockaddr *)&client_addr, &size);
			if (fd <= 0)
			{	
				printf("yes\n");
				break ;
			}
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