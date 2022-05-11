#include "HttpContext.hpp"

static Connection*	peer;
static epoll_event	events[256];

HttpContext::HttpContext(Multiplexer::events_t& events, int& fd) : _multiplexer(events, fd)
{ std::cout << "Creating webserver!" << std::endl;}

const Multiplexer&	HttpContext::getMultiplexer() const
{ return _multiplexer; }

bool	HttpContext::_mod_client() {
	const Server*	serv;
	Client*	client;

	client = peer->getClient();
	serv = client->search_requested_domain();
	client->set_response(new Response(serv, client->get_request()));
	if (_multiplexer.modEvent(peer, EPOLLOUT) == FAILURE)
		throw std::runtime_error("Failure epoll_mod\n");
	return SUCCESS;
}

bool	HttpContext::_del_client() {
	std::cout << "Client deleted from epoll!" << std::endl;
	if (_multiplexer.delEvent(peer) == FAILURE)
		throw std::runtime_error("Failure epoll_del\n");
	return SUCCESS;
}

bool	HttpContext::_add_client(int fd) {
	HttpContext::events_t	events;
	Connection*	connex;
	Client*		client;

	client = new Client(peer->getServers());
	client->set_request(new Request());
	connex = new Connection(fd, CLIENT, client);
	events = getMultiplexer().getEvents();
	events.insert(std::make_pair(fd, connex));
	if (_multiplexer.addEvent(connex, EPOLLIN | EPOLLET) == FAILURE)
		throw std::runtime_error("Failure epoll_add\n");
	return SUCCESS;
}

bool	HttpContext::new_connection() {
	sockaddr_in	addr;
	socklen_t	size;
	int			fd;

	size = sizeof(addr);
	bzero(&addr, size);
	if (peer->getType() == LISTENNER) {
		while ((fd = accept(peer->getFd(), (sockaddr *)&addr, &size)) > 0) {
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
void	HttpContext::manage_event(int id) {
	Client*	client;
	char	buf[256];
	int		state;
	int		rlen;

	client = peer->getClient();
	if (events[id].events & EPOLLIN) {
		state = client->get_state();
		while ((rlen = recv(peer->getFd(), buf, 255, 0)) > 0) {
			buf[rlen] = 0;
			client->process_req(buf);
		}
		if (rlen == 0)
			_del_client();
		else if (state & RESPONSE || state & ERROR)
			_mod_client();
	}
	if (events[id].events & EPOLLOUT) {
		client->process_res();
		peer->send(client->raw_data);
		if (client->raw_data.find("Connection : close"))
			_del_client();
	}
}

void	HttpContext::worker(void) {
	int	epoll;
	int	nfds;

	epoll = _multiplexer.getInstance();
	while (1) {
		if ((nfds = epoll_wait(epoll, events, 256, 1000)) < 0)
			throw std::runtime_error("Failure epoll_wait\n");
		for (int i = 0; i < nfds; ++i) {
			try {
				peer = reinterpret_cast<Connection*>(events[i].data.ptr);
				if (new_connection() == true)
					continue ;
				manage_event(i);
			} catch (...) {
				throw ;
			}
		}
	}
}