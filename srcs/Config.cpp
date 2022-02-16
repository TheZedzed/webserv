#include "Config.hpp"

Config::~Config() {
	Locations::iterator	it;

	std::cout << "Destroy current server config..." << std::endl;
	for (it = _locations.begin(); it != _locations.end(); ++it) {
		delete it->second; // calls ~Location()
	}
}

const Config::Locations&	Config::getLocation() const
{ return _locations; }

const ErrPage&	Config::getErrPages() const
{ return _error_page; }

const Array&	Config::getInclude() const
{ return _include; }

const Array&	Config::getIndexes() const
{ return _index; }

const Array&	Config::getNames() const
{ return _names; }

const String&	Config::getRoot() const
{ return _root; }

const Redirect&	Config::getRedir() const
{ return _ret; }

const size_t	Config::getMax() const
{ return _max; }

const bool		Config::getAuto() const
{ return _auto; }

std::ostream&	operator<<(std::ostream& out, const Config& config) {
	Config::Locations::const_iterator	it;
	ErrPage::const_iterator				it2;
	Array::const_iterator				it3;

	out << "[location directive]\n";
	it = config.getLocation().begin();
	for (; it != config.getLocation().end(); ++it) {
		out << "URL:" << it->first;
		out << "\nContent:\n" << it->second;
	}
	out << "\n[error_page directive]:\n";
	it2 = config.getErrPages().begin();
	for (; it2 != config.getErrPages().end(); ++it2) {
		out << "code: " << it->first;
		out << "\nuri: " << it->second;
	}
	out << "\n[server_name directive]\n";
	it3 = config.getNames().begin();
	for (; it3 != config.getNames().end(); ++it3) {
		out << *it3 << " ";
	}
	out << "\n[index directive]:\n";
	it3 = config.getIndexes().begin();
	for (; it3 != config.getIndexes().end(); ++it3) {
		out << *it3 << " ";
	}
	out << "\n[include directive]:\n";
	it3 = config.getInclude().begin();
	for (; it3 != config.getInclude().end(); ++it3) {
		out << *it3 << "\n";
	}
	out << "\n[client_max_body_size]: ";
	out << config.getMax();
	out << "\n[root directive]: ";
	out << config.getRoot();
	out << "\n[return directive]:\n";
	out << config.getRedir();
	out << "\n[autoindex directive]:\n";
	out << config.getAuto();
	return out;
}