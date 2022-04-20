#pragma once

# ifndef EVENT_HPP
# define EVENT_HPP

# include "Server.hpp"

/*
** class Event:
** manage I/O multiplexing
** manage pool of events:
**	manage fd in an epoll instance
**	use a map of socket + servers
*/
class	Event {
	public:
		typedef std::vector<Server*>	Servers;
		typedef std::map<int, Servers>	Pool;

		~Event();
		Event(Pool& events, int fd);

		bool	addEvent(int fd, int flag) const;
		bool	delEvent(int fd) const;
		bool	modEvent(int fd) const;

		int			getInstance() const;
		const Pool&	getEvents() const;

	private:
		Event&	operator=(const Event&);
		Event(const Event&);

		int		_epoll;
		Pool&	_events; // Contain map of pair(socket listen, array of server*)
};
#endif

