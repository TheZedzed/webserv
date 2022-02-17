#include "Event.hpp"

Event::Event(const Pool& events, int fd) : _events(events), _epoll(fd)
{ std::cout << "Create event.." << std::endl; }

Event::~Event() {
	Pool::const_iterator	it;

	std::cout << "Destroy Servers.." << std::endl;
	for (it = _events.begin(); it != _events.end(); ++it) {
		delete it->second;// calls ~Server()
		close(it->first);
	}
}

const int	Event::getInstance() const
{ return _epoll; }

const Event::Pool&	Event::getEvents() const
{ return _events; }

int	Event::newConnection(int socket) const {
	Pool::const_iterator	listen_fd;
	struct sockaddr_in	client_addr;
	socklen_t	size;
	int			new_socket;
	int			res;

	listen_fd = _events.find(socket);
	if (listen_fd != _events.end()) {
		while (42) { // loop cuz ET mode epoll
			new_socket = accept(listen_fd->first, reinterpret_cast<sockaddr *>(&client_addr), &size);
			if (new_socket <= 0)
				break ;
			res = addEvent(new_socket, EPOLLIN | EPOLLET);
			if (res)
				return -1;
		}
		return 1;
	}
	return 0;
}

bool	Event::modEvent(int fd) const {
	struct sockaddr_in	client_addr;
	struct epoll_event	ev;
	socklen_t	size;
	int			res;

	ev.data.fd = fd;
	ev.events |= EPOLLOUT;
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