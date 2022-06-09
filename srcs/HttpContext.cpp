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
		it->second->get_client()->set_state(DECONNECT);
		close(it->second->get_fd());
	}
	return ;
}

void	HttpContext::_mod_client() {
	const Server*	serv;
	Response*		res;
	Client*			cli;
	int				flag;

	flag = EPOLLOUT;
	cli = peer->get_client();
	if (cli->get_state() & RQLINE)
		flag = EPOLLIN | EPOLLET;
	else {
		serv = cli->search_requested_domain();
		res = new Response(serv, cli->get_request(), cli->raw_data);
		cli->set_response(res);
	}
	_multiplexer.mod_event(peer, flag);
	peer->arm_timer();
	return ;
}

void	HttpContext::_add_client(int fd) {
	Connection*	connex;
	Client*		client;
	timer_t		timer;

	client = new Client(peer->get_servers());
	connex = new Connection(fd, CLIENT, client);
	timer = connex->get_timer();
	_multiplexer.get_timers().insert(std::make_pair(timer, connex));
	_multiplexer.get_events().insert(std::make_pair(fd, connex));
	_multiplexer.add_event(connex, EPOLLIN | EPOLLET);
	return ;
}

bool	HttpContext::new_connection() {
	sockaddr_in	addr;
	socklen_t	size;
	int		listen_fd;
	int		fd;

	size = sizeof(addr);
	bzero(&addr, size);
	listen_fd = peer->get_fd();
	if (peer->get_type() == LISTENNER) {
		while (1) {
			fd = accept(listen_fd, reinterpret_cast<sockaddr *>(&addr), &size);
			if (fd <= 0)
				break ;
			_add_client(fd);
		}
		if (fd == -1 && errno != EAGAIN && errno != EWOULDBLOCK)
			throw std::runtime_error("Failure accept");
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
	int	epoll = _multiplexer.get_instance();
	int	state;
	int	nfds;

	while (1) {
		if ((nfds = epoll_wait(epoll, events, 256, 1000)) < 0)
			continue ;
		for (int i = 0; i < nfds; ++i) {
			peer = reinterpret_cast<Connection*>(events[i].data.ptr);
			if (new_connection() == true)
				continue ;
			if (peer->get_client()->get_state() & DECONNECT)
				continue ;
			if (events[i].events & EPOLLIN) {
				state = peer->retrieve_request();
				if (state & RESPONSE || state & ERROR)
					_mod_client();
			}
			if (events[i].events & EPOLLOUT)
				if (peer->send_and_close() == false)
					_mod_client();
		}
		_multiplexer.remove_deconnection();
	}
}
