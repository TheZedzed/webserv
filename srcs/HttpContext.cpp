#include "HttpContext.hpp"

static Connection*	peer;
static epoll_event	events[256];

HttpContext::HttpContext(const char* conf_file) {
	std::cout << "Creating webserver!" << std::endl;
	_parser = new Parser(conf_file);
	if (_multiplexer.build_events(_parser->get_map()) == FAILURE)
		throw std::runtime_error("Failed init multiplexing");
	if (_multiplexer.start_listenning() == FAILURE)
		throw std::runtime_error("Failed up servers");
	_init();
}

HttpContext::~HttpContext()
{ delete _parser; }

void	HttpContext::timeout(void* ptr) {
	timers_t::const_iterator	it;
	timer_t*	timerid;

	timerid = reinterpret_cast<timer_t*>(ptr);
	std::cout<< "Connection timeout!" << std::endl;
	it = _timers.find(*timerid);
	peer = it->second;
	_del_client();
	return ;
}

bool	HttpContext::_mod_client() {
	const Server*	serv;
	Client*			client;

	client = peer->get_client();
	serv = client->search_requested_domain();
	client->set_response(new Response(serv, client->get_request(), client->raw_data));
	if (_multiplexer.mod_event(peer, EPOLLOUT) == FAILURE)
		throw std::runtime_error("Failure epoll_mod\n");
	return SUCCESS;
}

bool	HttpContext::_del_client() {
	std::cout << "Client deleted from epoll!" << std::endl;
	if (timer_delete(peer->get_timer()) == -1)
		throw std::logic_error("Failure delete timer");
	if (_multiplexer.del_event(peer) == FAILURE)
		throw std::runtime_error("Failure epoll_del\n");
	return SUCCESS;
}

bool	HttpContext::_add_client(int fd) {
	Connection*	connex;
	Client*		client;

	client = new Client(peer->get_servers());
	client->set_request(new Request());
	connex = new Connection(fd, CLIENT, client);
	_timers.insert(std::make_pair(connex->get_timer(), connex));
	_multiplexer.get_events().insert(std::make_pair(fd, connex));
	if (_multiplexer.add_event(connex, EPOLLIN | EPOLLET) == FAILURE)
		throw std::runtime_error("Failure epoll_add\n");
	return SUCCESS;
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
			throw std::runtime_error("Failure accept\n");
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
			if (events[i].events & EPOLLIN) {
				state = peer->retrieve_request();
				if (state == DECONNECT)
					_del_client();
				else if (state & RESPONSE || state & ERROR)
					_mod_client();
			}
			if (events[i].events & EPOLLOUT) {
				if (peer->send_and_close() == true)
					_del_client();
			}
		}
	}
}
