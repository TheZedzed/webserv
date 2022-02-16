#include "Config.hpp"

const std::map<std::string, Location*>	Config::getLocation() const {
	return _locations;
}
