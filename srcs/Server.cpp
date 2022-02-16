#include "Server.hpp"

Server::~Server() {
	std::cout << "Destroy Server config.." << std::endl;
	delete _config;
}

const Config*	Server::getConfig() const
{ return _config; }

std::ostream&	operator<<(std::ostream& out, const Server& server) {
	out << server.getConfig();
}