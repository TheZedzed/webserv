#include "Server.hpp"

Server::Server (Config* config) : _config(config) {
	std::cout << "Create Server" << std::endl;
	/*
	** fill _config with config if not empty
	*/
}
Server::~Server() {
	std::cout << "Destroy Server config.." << std::endl;
	delete _config;
}

const Config*	Server::getConfig() const
{ return _config; }

std::ostream&	operator<<(std::ostream& out, const Server& server) {
	out << server.getConfig();
}