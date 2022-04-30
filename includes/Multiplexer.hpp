#pragma once

# ifndef MULTIPLEXER_HPP
# define MULTIPLEXER_HPP

# include "Connection.hpp"

/*
** class Multiplexer:
** manage I/O multiplexing
** store epoll instance
** store active sockets and their connection
*/
class	Multiplexer {
	public:
		typedef std::map<short, Connection*>	events_t;
		typedef Connection::servers_t			servers_t;

		~Multiplexer();
		Multiplexer(events_t& events, int fd);

		bool	addEvent(Connection* data, int flag);
		bool	modEvent(Connection* data, int flag);
		bool	delEvent(Connection* data);

		const int&		getInstance() const;
		const events_t&	getEvents() const;

	private:
		Multiplexer(const Multiplexer&);
		Multiplexer&	operator=(const Multiplexer&);

		int			_instance;
		events_t	_events;
};

#endif

