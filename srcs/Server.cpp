#include "Server.hpp"

Server::~Server() {
	std::cout << "Destroy Server config.." << std::endl;
	delete _config;
}