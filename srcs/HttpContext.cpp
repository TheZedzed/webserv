#include "HttpContext.hpp"

static Connection*	peer;
static epoll_event	events[256];

HttpContext::HttpContext(const char* conf_file) {
	std::cout << "Creating webserver!" << std::endl;
	_parser = new Parser(conf_file);
	_multiplexer.build_events(_parser->get_map());
	_multiplexer.start_listenning();
	_init();
}

HttpContext::~HttpContext()
{ delete _parser; }

void	HttpContext::timeout(void* ptr) {
	Multiplexer::timers_t::const_iterator	it;
	timer_t*	timerid;

	std::cout<< "Connection timeout!" << std::endl;
	timerid = reinterpret_cast<timer_t*>(ptr);
	it = _multiplexer.get_timers().find(*timerid);
	if (it != _multiplexer.get_timers().end()) {
		it->second->_state = DECONNECT;
		close(it->second->get_socket());
	}
	return ;
}

bool	HttpContext::mod_connection() {
	int	state;

	state = peer->_state;
	if (!(state & DECONNECT)) {
		if (state & RESET || state & RESPONSE)
			_mod_client();
		return false;
	}
	return true;
}

void	HttpContext::_mod_client() {
	int	flag;

	if (peer->_state & RESET) {
		peer->_state = RQLINE;
		flag = EPOLLIN;
	}
	else
		flag = EPOLLOUT;
	_multiplexer.mod_event(peer, flag);
	peer->arm_timer();
	return ;
}

void	HttpContext::_add_client(int fd) {
	Connection*	connex;

	fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);
	connex = new Connection(fd, CLIENT, peer->get_servers());
	_multiplexer.get_timers().insert(std::make_pair(connex->_timerid, connex));
	_multiplexer.get_events().insert(std::make_pair(fd, connex));
	_multiplexer.add_event(connex, EPOLLIN);
	return ;
}

bool	HttpContext::new_connection() {
	sockaddr_in	addr;
	socklen_t	size;
	int		listen_fd;
	int		fd;

	size = sizeof(addr);
	bzero(&addr, size);
	listen_fd = peer->get_socket();
	if (peer->get_type() == SERVERS) {
		fd = accept(listen_fd, reinterpret_cast<sockaddr *>(&addr), &size);
		if (fd == -1)
			throw std::runtime_error("Failure accept");
		_add_client(fd);
		return true;
	}
	return false;
}

/*
** Aim: manage event in epoll
** handle peer request (EPOLLIN)
** send a response to the peer (EPOLLOUT)
*/
void	HttpContext::worker(void) {
	int	epoll;
	int	nfds;

	epoll = _multiplexer.get_instance();
	while (1) {
		if ((nfds = epoll_wait(epoll, events, 256, 5000)) < 0)
			continue ;
		for (int i = 0; i < nfds; ++i) {
			peer = reinterpret_cast<Connection*>(events[i].data.ptr);
			if (new_connection() == true)
				continue ;
			if (events[i].events & EPOLLIN)
				peer->retrieve_request();
			else if (events[i].events & EPOLLOUT)
				peer->send_response();
			mod_connection();
		}
		_multiplexer.remove_deconnection();
	}
}
