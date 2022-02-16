#pragma once

# ifndef EVENT_HPP
# define EVENT_HPP

# include "Server.hpp"

class	Event {
	public:
		~Event();// destroy each Server from _servers
		Event(const std::vector<Server*>& servers);
		bool	addEvent(Server* server, int socket, int flag);// add an event based on one socket to the epoll instance
		bool	newEvent(void);// create epoll instance and add events (sockets with asociated server)
		bool	delEvent(int fd);
		bool	modEvent(epoll_event old, int fd);
		int		newConnection(int socket);
		bool	loop(void);
		const int						getEpoll() const;
		const std::vector<Server*>&		getServers() const;
	private:
		Event();
		Event&	operator=(const Event&);
		Event(const Event&);

		int		_epoll_fd;
		const std::vector<Server*>&	_servers;
};
#endif

