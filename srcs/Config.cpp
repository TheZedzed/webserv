#include "Config.hpp"

Config::~Config() {
	Locations::iterator	it;

	std::cout << "Destroy current server config..." << std::endl;
	for (it = _locations.begin(); it != _locations.end(); ++it) {
		delete it->second; // calls ~Location()
	}
}

const typename Config::Locations	Config::getLocation() const
{ return _locations; }
