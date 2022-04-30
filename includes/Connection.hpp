#pragma once

# ifndef CONNECTION_HPP
# define CONNECTION_HPP

# include "Client.hpp"

class	Connection {
	public:
		typedef Client::servers_t	servers_t;

		Connection(int fd, int type);
		~Connection();

		Client*				getClient();
		int					getFd() const;
		bool				getType() const;
		const servers_t&	getServers() const;

		void	setData(const servers_t& servers);
		void	setData(Client* client);

	private:
		union	data_u {
			servers_t	_servers;
			Client*		_client;
			~data_u() {} // must be declared
			data_u() {}
		};

		const int	_fd;
		const bool	_type;
		data_u		_data;
};

#endif
