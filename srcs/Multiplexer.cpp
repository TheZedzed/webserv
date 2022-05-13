#include "Multiplexer.hpp"

Multiplexer::Multiplexer(events_t& events, int fd) : _instance(fd), _events(events)
{ std::cout << "Init multiplexer.." << std::endl; }

Multiplexer::~Multiplexer() {
	events_t::const_iterator	it1;

	std::cout << "Destroy multiplexer.." << std::endl;
	for (it1 = _events.begin(); it1 != _events.end(); ++it1) {
		del_event(it1->second);
	}
	_events.clear();
}

const int&	Multiplexer::get_instance() const
{ return _instance; }

const Multiplexer::events_t&	Multiplexer::get_events() const
{ return _events; }

bool	Multiplexer::mod_event(Connection* el, int flag) {
	struct epoll_event	ev;
	int	res;

	ev.data.ptr = el;
	ev.events = flag;
	res = epoll_ctl(_instance, EPOLL_CTL_MOD, el->get_fd(), &ev);
	if (res)
		return FAILURE;
	return SUCCESS;
}

bool	Multiplexer::del_event(Connection* el) {
	int	res;
	int	fd;

	fd = el->get_fd();
	res = epoll_ctl(_instance, EPOLL_CTL_DEL, fd, NULL);
	if (res == -1)
		return FAILURE;
	delete el;
	close(fd);
	return SUCCESS;
}

bool	Multiplexer::add_event(Connection* el, int flag) {
	struct epoll_event	ev;
	int		res;

	ev.data.ptr = el;
	ev.events = flag;
	res = epoll_ctl(_instance, EPOLL_CTL_ADD, el->get_fd(), &ev);
	if (res == -1)
		return FAILURE;
	return SUCCESS;
}
