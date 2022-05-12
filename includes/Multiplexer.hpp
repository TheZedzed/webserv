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

		bool	add_event(Connection* data, int flag);
		bool	mod_event(Connection* data, int flag);
		bool	del_event(Connection* data);

		const int&		get_instance() const;
		const events_t&	get_events() const;

	private:
		Multiplexer(const Multiplexer&);
		Multiplexer&	operator=(const Multiplexer&);

		int			_instance;
		events_t	_events;
};

#endif

