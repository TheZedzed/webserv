#pragma once

# ifndef SERVER_HPP
# define SERVER_HPP

# include "Config.hpp"

/* manage socket, new connection on server */
class	Server {
	public:
		Server(const Config* config);
		~Server();

		const Config*	getConfig() const;

	private:
		Server(const Server&);
		Server&	operator=(const Server&);

		const Config*	_config;
};

std::ostream&	operator<<(std::ostream& out, const Server& server);
#endif