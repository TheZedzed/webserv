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

		void	setErrorPage(const strs_t& line);
		void	setRoute(const str_t& key, Location** loc);
		void	setSocket(const socket_t& socket);
		void	setNames(const strs_t& line);
		void	setMax(const str_t& line);
		void	sanitize_sockets(void);

		const pages_t&		getErrPages() const;
		const sockets_t&	getSockets() const;
		const route_t&		getRoutes() const;
		const strs_t&		getNames() const;
		size_t				getMax() const;

	private:
		Server(const Server&);
		Server&	operator=(const Server&);

		pages_t		_error_pages; // default: none
		sockets_t	_sockets; // default: one
		route_t		_routes; // default: none
		strs_t		_names; // default: none;
		size_t		_max; // default: 1024;
};

std::ostream&	operator<<(std::ostream& out, const Server& Server);
#endif