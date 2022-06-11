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

		int			get_fd() const;
		bool		get_type() const;
		Client*		get_client();
		const servers_t&	get_servers() const;

		void	arm_timer();
		void	send_response();
		void	retrieve_request();

		int		_state;
		timer_t	_timerid;

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

		const int	_fd;
		const bool	_type;
		data_u		_data;
};

#endif
