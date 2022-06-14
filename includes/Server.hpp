#pragma once

# ifndef SERVER_HPP
# define SERVER_HPP

# include "Location.hpp"

/* class Server:
** store mandatory config
*/
class	Server {
	public:
		typedef std::map<str_t, Location*>	route_t;
		typedef std::map<int, str_t>		pages_t;

		Server();
		~Server();

		void	set_err_page(const strs_t& line);
		void	set_route(const str_t& key, Location** loc);
		void	set_socket(const socket_t& socket);
		void	set_names(const strs_t& line);
		void	set_max(const str_t& line);
		void	sanitize_sockets(void);

		const pages_t&		get_err_pages() const;
		const sockets_t&	get_sockets() const;
		const route_t&		get_routes() const;
		const strs_t&		get_names() const;
		size_t				get_max() const;

	private:
		Server(const Server&);
		Server&	operator=(const Server&);

		pages_t		_error_pages; // default: none
		sockets_t	_sockets; // default: one
		route_t		_routes; // default: none
		strs_t		_names; // default: none;
		size_t		_max; // default: 1024;
};

const inline Server::route_t&	Server::get_routes() const
{ return _routes; }

const inline Server::pages_t&	Server::get_err_pages() const
{ return _error_pages; }

const inline sockets_t&	Server::get_sockets() const
{ return _sockets; }

const inline strs_t&	Server::get_names() const
{ return _names; }

inline size_t	Server::get_max() const
{ return _max; }

std::ostream&	operator<<(std::ostream& out, const Server& Server);
#endif