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
		typedef std::map<timer_t, Connection*>	timers_t;
		typedef std::map<short, Connection*>	events_t;
		typedef Parser::listenners_t		listenners_t;

		~Multiplexer();
		Multiplexer();

		int	get_instance() const;
		events_t&	get_events();
		timers_t&	get_timers();

		void	build_events(const listenners_t& map);
		void	remove_deconnection();
		void	start_listenning();

		void	add_event(Connection* data, int flag);
		void	mod_event(Connection* data, int flag);
		void	del_event(Connection* data);

	private:
		Multiplexer(const Multiplexer&);
		Multiplexer&	operator=(const Multiplexer&);

		int			_instance;
		events_t	_events;
		timers_t	_timers;
};

inline int	Multiplexer::get_instance() const
{ return _instance; }

inline Multiplexer::events_t&	Multiplexer::get_events()
{ return _events; }

inline Multiplexer::timers_t&	Multiplexer::get_timers()
{ return _timers; }

#endif

