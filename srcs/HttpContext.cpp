#include "HttpContext.hpp"

static const int	MAX_EVENTS = 256;
static const int	BUFFER_SIZE = 4096;
static epoll_event	events[MAX_EVENTS];

HttpContext::HttpContext(Parser* parser, Event::Pool& events, int& fd) : _server_info(parser), _multiplexer(events, fd)
{ std::cout << "Creating webserver..." << std::endl;}

HttpContext::~HttpContext() {
	std::cout << "Destroy webserver" << std::endl;
	delete _server_info;
}

const Event&	HttpContext::getMultiplexer() const
{ return _multiplexer; }

/*

bool	HttpContext::handleResponse(int socket) {

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
	if (_multiplexer.delEvent(socket))
		return FAILURE;
	return SUCCESS;
}

bool	HttpContext::handleResponse(int socket) {
	Event::Pool	events_pool	= getMultiplexer().getEvents();
	Response*	response = nullptr;
	Server*		cur_serv = nullptr;

	// search for servers listenning on the socket
	Event::Pool::iterator	servers	= events_pool.find(socket);

	// iterator on the first server
	Event::Servers::const_iterator	curr_serv = (*servers).second.begin();
	
	response = new Response(req, curr_serv, _err_code);

	//search for host in servers
	for (; curr_serv != (*servers).second.end(); ++curr_serv) { // loop on servers
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
	send(socket, response->getData(), response->getSize(), 0));
	delete response;
	return SUCCESS;
}

*/

bool	HttpContext::handleRequest(int socket) {
	char	buffer[BUFFER_SIZE] = {0};
	String	raw_data;
	int		rlen;

	while (26) {
		rlen = recv(socket, &buffer, BUFFER_SIZE - 1, 0);
		if (rlen == 0) { //peer disconnect
			if (_multiplexer.delEvent(socket))
				return FAILURE;
			break ;
		}
		else if (rlen == -1) { //finish read with non block fd (EAGAIN or EWOULDBLOCK)
			if (_multiplexer.modEvent(socket))
				return FAILURE;
			break ;
		}
		raw_data.append(buffer, rlen); // request recv
	}
	_request = new Request(raw_data);
	return SUCCESS;
}

int	HttpContext::newConnection(int socket) const {
	Event::Pool::const_iterator	listen_fd;
	struct sockaddr_in	client_addr;
	socklen_t	size;
	int			fd;

	size = sizeof(client_addr);
	bzero(&client_addr, size);
	listen_fd = _multiplexer.getEvents().find(socket);
	if (listen_fd != _multiplexer.getEvents().end()) {
		printf("New connection\n");
		while (42) { // loop cuz ET mode epoll
			fd = accept(listen_fd->first, (sockaddr *)&client_addr, &size);
			if (fd <= 0)
				break ;
			if (_multiplexer.addEvent(fd, EPOLLIN | EPOLLET))
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
		nfds = epoll_wait(_multiplexer.getInstance(), events, MAX_EVENTS, 1000);
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
				if (handleResponse(peer_fd))
					return FAILURE;
			}
		}
	}
	return SUCCESS;
}