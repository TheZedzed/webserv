#pragma once

# ifndef CONNECTION_HPP
# define CONNECTION_HPP

# include "Client.hpp"

class	Connection {
	public:
		typedef Parser::servers_t	servers_t;

		Connection(int fd, int type, const servers_t& servers);
		~Connection();

		bool	get_type() const;
		int		get_socket() const;
		const Client*	get_client() const;
		const servers_t&	get_servers() const;

		void	arm_();
		void	send_response();
		void	retrieve_request();

		int		_state; // connection state
		timer_t	_timerid; // connection timer

	private:
		Connection();
		Connection(const Connection&);
		Connection&	operator=(const Connection&);

		const Server*	_requested_server() const;

		Client*		_cli; // connection is client or NULL
		const int	_type; // client or servers
		const int	_socket; // socket listenning
		const servers_t&	_servers; // servers listennig on _socket
};

inline int	Connection::get_socket() const
{ return _socket; }

inline bool	Connection::get_type() const
{ return _type; }

const inline Client*	Connection::get_client() const
{ return _cli; }

const inline Connection::servers_t&	Connection::get_servers() const
{ return _servers; }

#endif
