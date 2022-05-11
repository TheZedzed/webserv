#pragma once

# ifndef CONNECTION_HPP
# define CONNECTION_HPP

# include "Client.hpp"

class	Connection {
	public:
		typedef Client::servers_t	servers_t;

		Connection(int fd, int type, Client* client);
		Connection(int fd, int type, const servers_t servers);
		~Connection();

		Client*	getClient();

		int					getFd() const;
		bool				getType() const;
		const servers_t&	getServers() const;

		bool	send_and_close();
		int		retrieve_request();

	private:
		Connection();
		Connection(const Connection&);
		Connection&	operator=(const Connection&);

		union	data_u {
			servers_t	_servers;
			Client*		_client;
			~data_u() {}
			data_u() {}
		};

		const int	_fd;
		const bool	_type;
		data_u		_data;
};

#endif
