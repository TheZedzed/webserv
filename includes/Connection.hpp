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
		bool				get_type() const;
		const servers_t&	get_servers() const;

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

		const int	_fd;
		const bool	_type;
		data_u		_data;
};

#endif
