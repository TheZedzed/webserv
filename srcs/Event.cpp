#include "Event.hpp"

Event::Event(const std::vector<Server*>& servers) : _servers(servers)
{ std::cout << "Create event.." << std::endl; }

const int	Event::getEpoll() const
{ return _epoll_fd; }

const std::vector<Server*>&	Event::getServers() const
{ return _servers; }

Event::~Event() {
	std::vector<Server*>::iterator	it;
	std::vector<Server*>			servers;

	servers = getServers();
	std::cout << "Destroy Servers.." << std::endl;
	for (it = servers.begin(); it != servers.end(); ++it) {
		delete *it;// calls ~Server()
	}
	servers.clear();
}

int	Event::newConnection(int socket) {
	std::vector<Server*>::const_iterator		cit;
	std::map<int, std::string>::const_iterator	listen_fd;

	struct sockaddr_in	client_addr;
	socklen_t	size;
	int			client_fd;
	int			new_socket;
	int	res;

	for (cit = getServers().begin(); cit != getServers().end(); ++cit) {
		listen_fd = (*cit)->getSockets().find(socket);
		if (listen_fd != (*cit)->getSockets().end()) {
			while ((new_socket = accept(listen_fd->first, reinterpret_cast<sockaddr *>(&client_addr), &size)) > 0) {//loop cuz ET mode
				res = addEvent(*cit, new_socket, EPOLLIN | EPOLLET);
				if (res)
					return -1;
			}
			return 1;
		}
	}
	return 0;
}

bool	Event::newEvent(void) {
	std::vector<Server*>::const_iterator		it1;
	std::map<int, std::string>::const_iterator	it2;
	int	res;

	_epoll_fd = epoll_create(1);
	if (_epoll_fd == -1)
		return FAILURE;
	for (it1 = getServers().begin(); it1 != getServers().end(); ++it1) {
		for (it2 = (*it1)->getSockets().begin(); it2 != (*it1)->getSockets().end(); ++it2) {
			res = addEvent(*it1, it2->first, EPOLLIN);
			if (res)
				return FAILURE;
		}
	}
	return SUCCESS;
}

bool	Event::modEvent(epoll_event old, int fd) {
	struct sockaddr_in	client_addr;
	struct epoll_event	ev;
	socklen_t	size;
	int			res;

	ev = old;
	ev.events |= EPOLLOUT;
	getsockname(fd, reinterpret_cast<sockaddr*>(&client_addr), &size);
	printf("client %s:%d request:\n%s\n", inet_ntoa(client_addr.sin_addr), client_addr.sin_port, data_recv.c_str());
	res = epoll_ctl(_epoll_fd, EPOLL_CTL_MOD, fd, &ev);
	if (res)
		return FAILURE;
	return SUCCESS;
}

bool	Event::delEvent(int fd) {
	struct sockaddr_in	client_addr;
	socklen_t	size;
	int			res;

	size = sizeof(client_addr);
	getsockname(fd, reinterpret_cast<sockaddr*>(&client_addr), (&size));
	printf("client:%s on port:%d connexion close!\n", inet_ntoa(client_addr.sin_addr), client_addr.sin_port);
	res = epoll_ctl(_epoll_fd, EPOLL_CTL_DEL, fd, NULL);
	if (res == -1)
		return FAILURE;
	close(fd);
	return SUCCESS;
}

bool	Event::addEvent(Server* server, int socket, int flag) {
	struct epoll_event	ev;
	int			res;

	if (server) {
		ev.data.fd = socket;
		ev.events = flag;
		ev.data.ptr = server;
		fcntl(socket, F_SETFL, O_NONBLOCK);
		res = epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, socket, &ev);
		if (res == -1)
			return FAILURE;
		return SUCCESS;
	}
	return FAILURE;
}