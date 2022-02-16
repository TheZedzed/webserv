#include "Server.hpp"

Server::~Server() {
	std::map<std::string, Location*>::iterator	it;
	std::map<std::string, Location*>	locations;

	locations = getLocation();
	std::cout << "Destroy locations in current server" << std::endl;
	for (it = locations.begin(); it != locations.end(); ++it) {
		delete it->second; // calls it->second destructor (~t_Location())
	}
	locations.clear();
}

const std::map<int,std::string>	Server::getSockets() const {
	return _config->_sockets;
}

const std::map<std::string, Location*>	Server::getLocation() const {
	return _config->_locations;
}
