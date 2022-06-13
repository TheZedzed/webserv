#include "Location.hpp"

Location::Location() : _cgi() {
	_auto = false;
	_root = "/html";
	_allow.push_back("GET");
	_redir = std::make_pair(-1, "");
}

Location::~Location()
{ std::cout << "Destroy current location directive" << std::endl; }

const str_t&	Location::get_root() const
{ return _root; }

const Location::redir_t&	Location::get_redir() const
{ return _redir; }

bool		Location::get_autoindex() const
{ return _auto; }

const strs_t&	Location::get_allowed() const
{ return _allow; }

const str_t&	Location::get_cgi() const
{ return _cgi; }

void	Location::set_root(const str_t& root)
{ _root = root; }

void	Location::set_autoindex(const str_t& autoindex)
{ _auto = (autoindex == "on" ? 1 : 0); }

void	Location::set_redirection(const strs_t& line) {
	redir_t	item;

	item = std::make_pair(std::atoi(line[1].c_str()), line[2]);
	_redir = item;
}

void	Location::set_method(const strs_t& line) {
	strs_t::const_iterator	it;

	it = line.begin() + 1;
	for (; it != line.end() - 1; ++it) {
		_allow.push_back(*it);
	}
}

void	Location::set_cgi(const str_t& cgi)
{ _cgi = cgi; }

std::ostream&	operator<<(std::ostream& out, const Location& loc) {
	strs_t::const_iterator	it;

	out << "\t[Location block]\n";
	out << "[Method allowed]: ";
	it = loc.get_allowed().begin();
	for (; it != loc.get_allowed().end(); ++it) {
		out << *it << " ";
	}
	out << "\n[root]: ";
	out << loc.get_root();
	out << "\n[autoindex]: ";
	out << loc.get_autoindex();
	out << "\n[redirection]:";
	out << loc.get_redir().first + " " + loc.get_redir().second;
	out << "\n[cgi]: ";
	out << loc.get_cgi();
	out << "\n";
	return out;
}