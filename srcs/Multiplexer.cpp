#include "Multiplexer.hpp"

Multiplexer::Multiplexer(events_t& events, int fd) : _instance(fd), _events(events)
{ std::cout << "Init multiplexer.." << std::endl; }

Multiplexer::~Multiplexer() {
	events_t::const_iterator	it1;

	std::cout << "Destroy multiplexer.." << std::endl;
	for (it1 = _events.begin(); it1 != _events.end(); ++it1) {
		delEvent(it1->second);
	}
	_events.clear();
}

const int&	Multiplexer::getInstance() const
{ return _instance; }

const Multiplexer::events_t&	Multiplexer::getEvents() const
{ return _events; }

bool	Multiplexer::modEvent(Connection* el, int flag) {
	struct epoll_event	ev;
	int	res;

	ev.data.ptr = el;
	ev.events = flag;
	res = epoll_ctl(_instance, EPOLL_CTL_MOD, el->getFd(), &ev);
	if (res)
		return FAILURE;
	return SUCCESS;
}

bool	Multiplexer::delEvent(Connection* el) {
	int	res;
	int	fd;

	fd = el->getFd();
	res = epoll_ctl(_instance, EPOLL_CTL_DEL, fd, NULL);
	if (res == -1)
		return FAILURE;
	close(fd);
	delete el;
	return SUCCESS;
}

bool	Multiplexer::addEvent(Connection* el, int flag) {
	struct epoll_event	ev;
	int		res;

	ev.data.ptr = el;
	ev.events = flag;
	res = epoll_ctl(_instance, EPOLL_CTL_ADD, el->getFd(), &ev);
	if (res == -1)
		return FAILURE;
	return SUCCESS;
}
