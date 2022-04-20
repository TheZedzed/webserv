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
	struct epoll_event	ev;
	int	res;

	ev.data.fd = fd;
	ev.events = EPOLLOUT;
	res = epoll_ctl(_epoll, EPOLL_CTL_MOD, fd, &ev);
	if (res)
		return FAILURE;
	return SUCCESS;
}

bool	Event::delEvent(int fd) const {
	socklen_t	size;
	int			res;

	res = epoll_ctl(_epoll, EPOLL_CTL_DEL, fd, NULL);
	if (res == -1)
		return FAILURE;
	close(fd);
	return SUCCESS;
}

bool	Event::addEvent(int fd, int flag) const {
	struct epoll_event	ev;
	int		res;

	ev.data.fd = fd;
	ev.events = flag;
	fcntl(fd, F_SETFL, O_NONBLOCK);
	res = epoll_ctl(_epoll, EPOLL_CTL_ADD, fd, &ev);
	if (res == -1)
		return FAILURE;
	return SUCCESS;
}