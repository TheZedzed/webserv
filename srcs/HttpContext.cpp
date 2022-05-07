#include "HttpContext.hpp"

static Connection*	peer;
static epoll_event	events[256];

HttpContext::HttpContext(Multiplexer::events_t& events, int& fd) : _multiplexer(events, fd)
{ std::cout << "Creating webserver!" << std::endl;}

const Multiplexer&	HttpContext::getMultiplexer() const
{ return _multiplexer; }

/*
** Aim: parse received data
** save data in peer buffer
** try extract request from this buffer
*/
bool	HttpContext::handle_request() {
	char	buf[256];
	Client*	client;
	int		rlen;

	client = peer->getClient();
	while ((rlen = recv(peer->getFd(), buf, 255, 0)) != -1) {
		if (rlen == 0)
			return _del_client();
		else {
			buf[rlen] = 0;
			peer->data_received.append(buf);
			client->process_req(peer->data_received);
		}
	}
	return SUCCESS;
}

bool	HttpContext::handle_response() {

}

bool	HttpContext::_mod_client(int code) {
	Response*	res;
	Client*		cli;

	cli = peer->getClient();
	res = new Response(code);
	cli->set_response(res);
	cli->set_flag(RESPONSE | RES_HEAD);
	if (_multiplexer.modEvent(peer, EPOLLOUT) == FAILURE)
		throw std::runtime_error("Failure epoll_mod\n");
	return SUCCESS;
}

bool	HttpContext::_del_client() {
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

void	HttpContext::manage_event(int id) {
	if (new_connection() == true)
		return ;
	else if (events[id].events & EPOLLIN) {
		if (handle_request() == FAILURE)
			throw 500;
	}
	else if (events[id].events & EPOLLOUT) {
		if (handle_response() == FAILURE) // to do
			throw 500;
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
				manage_event(i);
			} catch (int code) {
				_mod_client(code);
			} catch (...) {
				throw ;
			}
		}
	}
}