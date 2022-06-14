#include "Multiplexer.hpp"

static bool	_bind_socket(int fd, socket_t socket) {
	sockaddr_in	addr;

	bzero(&addr, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(inet_addr(socket.first.c_str()));
	addr.sin_port =  htons(_atoi(socket.second, 10));
	if (bind(fd, (sockaddr *)&addr, sizeof(sockaddr_in)))
		return FAILURE;
	return SUCCESS;
}

Multiplexer::Multiplexer()
{ std::cout << "Create multiplexer!" << std::endl; }

Multiplexer::~Multiplexer() {
	events_t::const_iterator	it1;
	events_t::const_iterator	save;

	std::cout << "Destroy multiplexer!" << std::endl;
	it1 = _events.begin();
	while (it1 != _events.end()) {
		save = it1;
		++save;
		del_event(it1->second);
		it1 = save;
	}
}

void	Multiplexer::build_events(const Parser::listenners_t& map) {
	Parser::listenners_t::const_iterator	it;
	Connection*	connex;
	int		socket_fd;
	int		opt;

	opt = 1;
	it = map.begin();
	for (; it != map.end(); ++it) {
		if ((socket_fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0)) < 0)
			break ;
		if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)))
			break ;
		if (_bind_socket(socket_fd, it->first) == FAILURE)
			break ;
		if (listen(socket_fd, 100))
			break ;
		connex = new Connection(socket_fd, SERVERS, it->second);
		_events.insert(std::make_pair(socket_fd, connex));
	}
	if (it != map.end())
		throw std::runtime_error("Failed init multiplexer");
	return ;
}

void	Multiplexer::start_listenning() {
	events_t::const_iterator	it;

	_instance = epoll_create(1);
	if (_instance == -1)
		throw std::runtime_error("Failure create epoll instance!");
	for (it = _events.begin(); it != _events.end(); ++it)
		add_event(it->second, EPOLLIN);
	return ;
}

void	Multiplexer::mod_event(Connection* el, int flag) {
	struct epoll_event	ev;

	ev.data.ptr = el;
	ev.events = flag;
	if (epoll_ctl(_instance, EPOLL_CTL_MOD, el->get_socket(), &ev) == -1)
		throw std::runtime_error("Failure epoll mod\n");
	return ;
}

void	Multiplexer::del_event(Connection* el) {
	int		fd;

	fd = el->get_socket();
	_events.erase(fd);
	if (el->get_type() == CLIENT)
		_timers.erase(el->_timerid);
	if (epoll_ctl(_instance, EPOLL_CTL_DEL, fd, NULL) == 1)
		throw std::runtime_error("Failure epoll del\n");
	delete el;
	return ;
}

void	Multiplexer::add_event(Connection* el, int flag) {
	struct epoll_event	ev;

	ev.data.ptr = el;
	ev.events = flag;
	if (epoll_ctl(_instance, EPOLL_CTL_ADD, el->get_socket(), &ev) == -1)
		throw std::runtime_error("Failure epoll add\n");
	return ;
}

void	Multiplexer::remove_deconnection() {
	events_t::const_iterator	save;
	events_t::const_iterator	it;
	Connection*	peer;

	it = _events.begin();
	while (it != _events.end()) {
		save = it;
		++save;
		peer = it->second;
		if (peer && peer->get_type() == CLIENT) {
			if (peer->_state == DECONNECT)
				del_event(it->second);
		}
		it = save;
	}
	return ;
}