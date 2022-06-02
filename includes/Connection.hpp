#pragma once

# ifndef CONNECTION_HPP
# define CONNECTION_HPP

# include "Client.hpp"

class	Connection {
	public:
		typedef Client::servers_t	servers_t;

		Connection(int fd, int type, Client* client);
		Connection(int fd, int type, const servers_t& servers);
		~Connection();

		Client*	get_client();

		int					get_fd() const;
		timer_t				get_timer() const;
		bool				get_type() const;
		const servers_t&	get_servers() const;

		bool	arm_timer();
		bool	create_timer();
		bool	send_and_close();
		int		retrieve_request();

	private:
		Connection();
		Connection(const Connection&);
		Connection&	operator=(const Connection&);

		union	data_u {
			Client*				_client;
			const servers_t*	_servers;
			~data_u() {}
			data_u() {}
		};

		timer_t		_timerid;
		const int	_fd;
		const bool	_type;
		data_u		_data;
};

#endif
