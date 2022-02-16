#pragma once

# ifndef SERVER_HPP
# define SERVER_HPP

# include "Config.hpp"

/* manage socket, new connection on server */
class	Server {
	public:
		Server();
		~Server(); // destroy each location in config
		Server&	operator=(const Server&);
		Server(const Server&);
		bool	addSocket(void);
	private:
		Config*	_config;
};
#endif