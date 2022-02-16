#include "Config.hpp"

Config::Config() : _routes(), _error_page() {
	_max = 1024;
	_names.push_back("localhost");
	_socket = std::make_pair("127.0.0.1", "80");
}

Config::~Config() {
	Route::iterator	it;

	std::cout << "Destroy current server config..." << std::endl;
	for (it = _routes.begin(); it != _routes.end(); ++it) {
		delete it->second; // calls ~Location()
	}
}

const Config::Route&	Config::getRoutes() const
{ return _routes; }

const ErrPage&	Config::getErrPages() const
{ return _error_page; }

const Array&	Config::getNames() const
{ return _names; }

const size_t	Config::getMax() const
{ return _max; }

const Socket&	Config::getSocket() const
{ return _socket; }

bool	Config::setSocket(const Array& line) {
	Socket	item;

	item = std::make_pair(line[1], line[2]);
	_socket = item;
}

bool	Config::setMax(const String& line)
{ _max = std::atoi(line.c_str()); }

bool	Config::setErrorPage(const Array& line) {
	std::pair<int, String>	item;
	String	value;
	int		key;

	key = std::atoi(line[1].c_str());
	value = line[2];
	item = std::make_pair(key, value);
	_error_page.insert(item);
}

bool	Config::setRoutes(const String& key, Location** loc) {
	std::pair<String, Location*>	item;

	item = std::make_pair(key, *loc);
	_routes.insert(item);
}

bool	Config::setNames(const Array& line) {
	Array::const_iterator	it;

	it = line.begin();
	for (; it != line.end(); ++it) {
		_names.push_back(*it);
	}
}

std::ostream&	operator<<(std::ostream& out, const Config& config) {
	Config::Route::const_iterator	it;
	ErrPage::const_iterator			it2;
	Array::const_iterator			it3;

	out << "[Config server info]\n";
	out << "\n[Socket]: ";
	out << config.getSocket();
	out << "\n[routes]:\n";
	it = config.getRoutes().begin();
	for (; it != config.getRoutes().end(); ++it) {
		out << "url: " << it->first;
		out << "\ndirectives:\n" << it->second;
	}
	out << "\n[error pages]:\n";
	it2 = config.getErrPages().begin();
	for (; it2 != config.getErrPages().end(); ++it2) {
		out << "code: " << it2->first;
		out << "\nuri: " << it2->second;
	}
	out << "\n[server_names]\n";
	it3 = config.getNames().begin();
	for (; it3 != config.getNames().end(); ++it3) {
		out << *it3 << " ";
	}
	out << "\n[client_max_body_size]: ";
	out << config.getMax();
	out << "\n";
	return out;
}