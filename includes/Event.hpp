#pragma once

# ifndef EVENT_HPP
# define EVENT_HPP

# include "Server.hpp"

class	Event {
	public:
		typedef typename std::map<int, Server*>	Pool;

		Event(const Pool& events, int fd);
		~Event();// destroy each event

		bool	addEvent(int socket, int flag) const;// add an event based on one socket to the epoll instance
		bool	newEvent() const;// create epoll instance and add events (sockets with asociated server)
		bool	delEvent(int fd) const;
		bool	modEvent(int fd) const;
		int		newConnection(int socket) const;
		bool	loop() const;

		const int	getInstance() const;
		const Pool&	getEvents() const;

	private:
		Event&	operator=(const Event&);
		Event(const Event&);

		int			_epoll;
		const Pool&	_events; // Contain map of pair(socket, server*)
};
#endif

