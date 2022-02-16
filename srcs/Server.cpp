#include "Server.hpp"

Server::~Server() {
	std::map<std::string, Location*>::iterator	it;
	std::map<std::string, Location*>	locations;

	locations = _config->getLocation();
	std::cout << "Destroy locations in current server" << std::endl;
	for (it = locations.begin(); it != locations.end(); ++it) {
		delete it->second; // destroy Location
	}
	locations.clear();
}