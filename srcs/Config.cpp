#include "Config.hpp"

Config::Config() : _error_page(), _routes() {
	_max = 1024;
	_names.push_back("");
	_sockets.push_back(std::make_pair("127.0.0.1", "80"));
}

Config::~Config() {
	Routes::iterator	it;

	std::cout << "Destroy current server config..." << std::endl;
	for (it = _routes.begin(); it != _routes.end(); ++it) {
		delete it->second; // calls ~Location()
	}
}

const Config::Routes&	Config::getRoutes() const
{ return _routes; }

const Config::ErrPage&	Config::getErrPages() const
{ return _error_page; }

const Config::Sockets&	Config::getSockets() const
{ return _sockets; }

const Array&	Config::getNames() const
{ return _names; }

size_t		Config::getMax() const
{ return _max; }

void	Config::setSockets(const Sockets& cpy) {
	_sockets = cpy;
}

void	Config::setMax(const String& line)
{ _max = std::atoi(line.c_str()); }

void	Config::setErrorPage(const Array& line) {
	String	value;
	int		key;

	value = line[2];
	key = std::atoi(line[1].c_str());
	_error_page.insert(std::make_pair(key, value));
}

void	Config::setRoutes(const String& key, Location** loc) {
	std::pair<String, Location*>	item;

	item = std::make_pair(key, *loc);
	_routes.insert(item);
}

void	Config::setNames(const Array& line) {
	Array::const_iterator	it;

	it = line.begin() + 1;
	for (; it != line.end(); ++it) {
		_names.push_back(*it);
	}
}



std::ostream&	operator<<(std::ostream& out, const Config::ErrPage& errors) {
	Config::ErrPage::const_iterator	it;

	out << "\n[Error pages]:\n";
	it = errors.begin();
	for (; it != errors.end(); ++it) {
		out << "code: " << it->first;
		out << "\nuri: " << it->second;
		out << "\n";
	}
	return out;
}

std::ostream&	operator<<(std::ostream& out, const Config::Sockets& sockets) {
	Config::Sockets::const_iterator	it;
	
	out << "\n[Sockets]:\n";
	it = sockets.begin();
	for (; it != sockets.end(); ++it) {
		out << it->first + ":" + it->second + "\n";
	}
	return out;
}

std::ostream&	operator<<(std::ostream& out, const Config::Routes& routes) {
	Config::Routes::const_iterator	it;
	
	out << "\n[Routes]:\n";
	it = routes.begin();
	for (; it != routes.end(); ++it) {
		out << "url: " << it->first;
		out << "\ndirectives:\n" << it->second;
	}
	return out;
}

std::ostream&	operator<<(std::ostream& out, const Config& config) {
	Array::const_iterator	it;

	out << "[Config server info]\n";
	out << config.getSockets();
	out << config.getRoutes();
	out << config.getErrPages();
	out << "\n[server_names]\n";
	it = config.getNames().begin();
	for (; it != config.getNames().end(); ++it) {
		out << *it << " ";
	}
	out << "\n[client_max_body_size]: ";
	out << config.getMax();
	out << "\n";
	return out;
}