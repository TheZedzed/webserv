#pragma once

# ifndef EVENT_HPP
# define EVENT_HPP

# include "Server.hpp"

class	Event {
	public:
		typedef std::vector<Server*>	Servers;
		typedef std::map<int, Servers>	Pool;

		~Event();
		Event(const Pool& events, int fd);

		bool	addEvent(int socket, int flag) const;
		bool	delEvent(int fd) const;
		bool	modEvent(int fd) const;

		int			getInstance() const;
		const Pool&	getEvents() const;

	private:
		Event&	operator=(const Event&);
		Event(const Event&);

		int			_epoll;
		const Pool&	_events; // Contain map of pair(socket, array of server*)
};
#endif

