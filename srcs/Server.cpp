#include "Server.hpp"

Server::Server(const Config* config) : _config(config) {
	std::cout << "[Create Server]\n";
	std::cout << "Config:\n" << config << std::endl;
}
Server::~Server() {
	std::cout << "[Destroy Server]" << std::endl;
	delete _config;
}

const Config*	Server::getConfig() const
{ return _config; }

std::ostream&	operator<<(std::ostream& out, const Server& server) {
	out << server.getConfig();
	return out;
}