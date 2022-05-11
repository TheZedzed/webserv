#include "Location.hpp"

Location::Location() : _cgi() {
	_auto = false;
	_root = "/html";
	_allow.push_back("GET");
	_redir = std::make_pair(-1, "");
}

Location::~Location()
{ std::cout << "Destroy current location directive" << std::endl; }

const str_t&	Location::getRoot() const
{ return _root; }

const Location::redir_t&	Location::getRedir() const
{ return _redir; }

bool		Location::getAuto() const
{ return _auto; }

const strs_t&	Location::getAllow() const
{ return _allow; }

const str_t&	Location::getCgi() const
{ return _cgi; }

void	Location::setRoot(const str_t& root)
{ _root = root; }

void	Location::setAutoIndex(const str_t& autoindex)
{ _auto = (autoindex == "on" ? 1 : 0); }

void	Location::setRedirection(const strs_t& line) {
	redir_t	item;

	item = std::make_pair(std::atoi(line[1].c_str()), line[2]);
	_redir = item;
}

void	Location::setMethod(const strs_t& line) {
	strs_t::const_iterator	it;

	it = line.begin() + 1;
	for (; it != line.end(); ++it) {
		_allow.push_back(*it);
	}
}

void	Location::setCgi(const str_t& cgi)
{ _cgi = cgi; }

std::ostream&	operator<<(std::ostream& out, const Location& loc) {
	strs_t::const_iterator	it;

	out << "\t[Location block]\n";
	out << "[Method allowed]: ";
	it = loc.getAllow().begin();
	for (; it != loc.getAllow().end(); ++it) {
		out << *it << " ";
	}
	out << "\n[root]: ";
	out << loc.getRoot();
	out << "\n[autoindex]: ";
	out << loc.getAuto();
	out << "\n[redirection]:";
	out << loc.getRedir().first + " " + loc.getRedir().second;
	out << "\n[cgi]: ";
	out << loc.getCgi();
	out << "\n";
	return out;
}