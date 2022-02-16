#pragma once

# ifndef SERVER_HPP
# define SERVER_HPP

# include "Config.hpp"

/* manage socket, new connection on server */
class	Server {
	public:
		Server();
		~Server(); // destroy Config

	private:
		Server(const Server&);
		Server&	operator=(const Server&);

		Config*	_config;
};
#endif