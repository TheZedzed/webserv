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
	_init_error_pages();
}

HttpContext::~HttpContext()
{ delete _parser; }

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
	_multiplexer.get_events().insert(std::make_pair(fd, connex));
	if (_multiplexer.add_event(connex, EPOLLIN | EPOLLET) == FAILURE)
		throw std::runtime_error("Failure epoll_add\n");
	return SUCCESS;
}

bool	HttpContext::new_connection() {
	sockaddr_in	addr;
	socklen_t	size;
	int			fd;

	size = sizeof(addr);
	bzero(&addr, size);
	if (peer->get_type() == LISTENNER) {
		while ((fd = accept(peer->get_fd(), (sockaddr *)&addr, &size)) > 0) {
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
			throw std::runtime_error("Failure epoll_wait\n");
		for (int i = 0; i < nfds; ++i) {
			peer = reinterpret_cast<Connection*>(events[i].data.ptr);
			if (new_connection() == true)
				continue ;
			if (events[i].events & EPOLLIN) {
				if ((state = peer->retrieve_request()) == DECONNECT) {
					_del_client();
					break ;
				}
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
