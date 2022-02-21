#include "Location.hpp"

Location::Location() : _allowed(), _cgi() {
	_auto = false;
	_root = "html";
	_redir = std::make_pair(-1, "");
}

Location::~Location()
{ std::cout << "Destroy current location directive" << std::endl; }

const String&	Location::getRoot() const
{ return _root; }

const Location::Redirect&	Location::getRedir() const
{ return _redir; }

bool		Location::getAuto() const
{ return _auto; }

const Array&	Location::getAllow() const
{ return _allowed; }

const String&	Location::getCgi() const
{ return _cgi; }

void	Location::setRoot(const String& root)
{ _root = root; }

void	Location::setAutoIndex(const String& autoindex)
{ _auto = (autoindex == "on" ? 1 : 0); }

void	Location::setRedirection(const Array& line) {
	Redirect	item;

	item = std::make_pair(std::atoi(line[1].c_str()), line[2]);
	_redir = item;
}

void	Location::setMethod(const Array& line) {
	Array::const_iterator	it;

	it = line.begin() + 1;
	for (; it != line.end(); ++it) {
		_allowed.push_back(*it);
	}
}

void	Location::setCgi(const String& cgi)
{ _cgi = cgi; }

std::ostream&	operator<<(std::ostream& out, const Location& loc) {
	Array::const_iterator	it;

	out << "\t[Location block]\n";
	out << "\n[allowed]: ";
	it = loc.getAllow().begin();
	for (; it != loc.getAllow().end(); ++it) {
		out << *it << " ";
	}
	out << "\n[root]: ";
	out << loc.getRoot();
	out << "\n[autoindex]: ";
	out << loc.getAuto();
	out << "\n[redirection]:";
	//out << loc.getRedir();
	out << "\n[cgi]: ";
	out << loc.getCgi();
	out << "\n";
	return out;
}