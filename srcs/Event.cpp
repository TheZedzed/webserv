#include "Event.hpp"

Event::Event(Pool& events, int fd) : _epoll(fd), _events(events)
{ std::cout << "Create event.." << std::endl; }

Event::~Event() {
	Servers::const_iterator	it1;
	Pool::iterator	it;

	std::cout << "Destroy Servers.." << std::endl;
	for (it = _events.begin(); it != _events.end(); ++it) {
		it1 = it->second.begin();
		for (; it1 != it->second.end(); ++it1) {
			delete *it1; // calls ~Server()
		}
		close(it->first); // close socket
	}
	_events.clear();
}

int		Event::getInstance() const
{ return _epoll; }

const Event::Pool&	Event::getEvents() const
{ return _events; }

bool	Event::modEvent(int fd) const {
	struct sockaddr_in	client_addr;
	struct epoll_event	ev;
	socklen_t	size;
	int			res;

	ev.data.fd = fd;
	ev.events = EPOLLOUT;
	size = sizeof(client_addr);
	bzero(&client_addr, sizeof(client_addr));
	getsockname(fd, reinterpret_cast<sockaddr*>(&client_addr), &size);
	printf("client %s:%d request:\n%s\n", inet_ntoa(client_addr.sin_addr), client_addr.sin_port, data_recv.c_str());
	res = epoll_ctl(_epoll, EPOLL_CTL_MOD, fd, &ev);
	if (res)
		return FAILURE;
	return SUCCESS;
}

bool	Event::delEvent(int fd) const {
	struct sockaddr_in	client_addr;
	socklen_t	size;
	int			res;

	size = sizeof(client_addr);
	bzero(&client_addr, sizeof(client_addr));
	getsockname(fd, reinterpret_cast<sockaddr*>(&client_addr), (&size));
	printf("client:%s on port:%d connexion close!\n", inet_ntoa(client_addr.sin_addr), client_addr.sin_port);
	res = epoll_ctl(_epoll, EPOLL_CTL_DEL, fd, NULL);
	if (res == -1)
		return FAILURE;
	close(fd);
	return SUCCESS;
}

bool	Event::addEvent(int socket, int flag) const {
	struct epoll_event	ev;
	int		res;

	ev.data.fd = socket;
	ev.events = flag;
	fcntl(socket, F_SETFL, O_NONBLOCK);
	res = epoll_ctl(_epoll, EPOLL_CTL_ADD, socket, &ev);
	if (res == -1)
		return FAILURE;
	return SUCCESS;
}