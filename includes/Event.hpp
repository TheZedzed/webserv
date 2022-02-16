#pragma once

# ifndef EVENT_HPP
# define EVENT_HPP

# include "Server.hpp"

class	Event {
	public:
		~Event();// destroy each event
		Event(std::map<int, Server*>& events);
		bool	addEvent(int socket, int flag);// add an event based on one socket to the epoll instance
		bool	newEvent();// create epoll instance and add events (sockets with asociated server)
		bool	delEvent(int fd);
		bool	modEvent(int fd);
		int		newConnection(int socket);
		bool	loop();
		const int	getInstance() const;
		const std::map<int, Server*>	getEvents() const;
	private:
		Event();
		Event&	operator=(const Event&);
		Event(const Event&);

		int		_epoll_fd;
		std::map<int, Server*>&	_events; // Contain pair(socket, server)
};
#endif

