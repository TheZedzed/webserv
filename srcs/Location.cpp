#include "Location.hpp"

Location::Location() : _allowed(), _cgi() {
	_auto = false;
	_root = "html";
	_redir = std::make_pair(-1, "");
}

Location::~Location()
{ std::cout << "Destroy current location directive.." << std::endl; }

const String&	Location::getRoot() const
{ return _root; }

const Redirect&	Location::getRedir() const
{ return _redir; }

const bool		Location::getAuto() const
{ return _auto; }

const Array&	Location::getAllow() const
{ return _allowed; }

const String&	Location::getCgi() const
{ return _cgi; }

std::ostream&	operator<<(std::ostream& out, const Location& loc) {
	Array::const_iterator	it;

	out << "[Location block]\n";
	out << "\n[allowed]: ";
	it = loc.getAllow().begin();
	for (; it != loc.getAllow().end(); ++it) {
		out << *it << " ";
	}
	out << "\n[root]: ";
	out << loc.getRoot();
	out << "\n[autoindex]: ";
	out << loc.getAuto();
	out << "\n[redirection]:\n";
	out << loc.getRedir();
	out << "\n[cgi]: ";
	out << loc.getCgi();
	out << "\n";
	return out;
}