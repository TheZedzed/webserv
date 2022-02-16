#pragma once

# ifndef SERVER_HPP
# define SERVER_HPP

# include "Config.hpp"

/* manage socket, new connection on server */
class	Server {
	public:
		Server();
		Server&	operator=(const Server&);
		Server(const Server&);
		virtual ~Server();// destroy each node in _locations
		const std::map<int, std::string>		getSockets() const;
		const std::map<std::string, Location*>	getLocation() const;
	private:
		Config*	_config;
};
#endif