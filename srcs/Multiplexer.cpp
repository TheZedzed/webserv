#include "Multiplexer.hpp"

Multiplexer::Multiplexer()
{ std::cout << "Init multiplexer.." << std::endl; }

Multiplexer::~Multiplexer() {
	events_t::const_iterator	it1;

	std::cout << "Destroy multiplexer.." << std::endl;
	for (it1 = _events.begin(); it1 != _events.end(); ++it1) {
		del_event(it1->second);
		close(it1->first);
	}
}

const int&	Multiplexer::get_instance() const
{ return _instance; }

Multiplexer::events_t&	Multiplexer::get_events()
{ return _events; }

/*
**
*/
bool	Multiplexer::build_events(const Parser::listenners_t& map) {
	Parser::listenners_t::const_iterator	it;
	Connection*	listenner;
	sockaddr_in	addr;
	int		socket_fd;
	int		opt;

	opt = 1;
	it = map.begin();
	for (; it != map.end(); ++it) {
		bzero(&addr, sizeof(addr));
		socket_fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
		setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
		addr.sin_family = AF_INET; // ipv4
		addr.sin_addr.s_addr = htonl(inet_addr(it->first.first.c_str()));
		addr.sin_port =  htons(_atoi(it->first.second, 10));
		if (bind(socket_fd, (sockaddr *)&addr, sizeof(sockaddr_in)))
			return FAILURE;
		if (listen(socket_fd, 100)) // up to 100 connections
			return FAILURE;
		listenner = new Connection(socket_fd, LISTENNER, it->second);
		_events.insert(std::make_pair(socket_fd, listenner));
	}
	return SUCCESS;
}

/*
** Aim: init epoll instance
** add servers socket in reading mode
*/
bool	Multiplexer::start_listenning() {
	events_t::const_iterator	it;
	struct epoll_event	ev;
	int	res;

	_instance = epoll_create(1);
	if (_instance == -1)
		return FAILURE;
	for (it = _events.begin(); it != _events.end(); ++it) {
		ev.data.ptr = it->second;
		ev.events = EPOLLIN;
		res = epoll_ctl(_instance, EPOLL_CTL_ADD, it->first, &ev);
		if (res == -1)
			return FAILURE;
	}
	return SUCCESS;
}

bool	Multiplexer::mod_event(Connection* el, int flag) {
	struct epoll_event	ev;
	int	res;

	ev.data.ptr = el;
	ev.events = flag;
	res = epoll_ctl(_instance, EPOLL_CTL_MOD, el->get_fd(), &ev);
	if (res)
		return FAILURE;
	return SUCCESS;
}

bool	Multiplexer::del_event(Connection* el) {
	int	res;

	res = epoll_ctl(_instance, EPOLL_CTL_DEL, el->get_fd(), NULL);
	if (res == -1)
		return FAILURE;
	delete el;
	return SUCCESS;
}

bool	Multiplexer::add_event(Connection* el, int flag) {
	struct epoll_event	ev;
	int		res;

	ev.data.ptr = el;
	ev.events = flag;
	res = epoll_ctl(_instance, EPOLL_CTL_ADD, el->get_fd(), &ev);
	if (res == -1)
		return FAILURE;
	return SUCCESS;
}
