#include "Location.hpp"

Location::~Location()
{ std::cout << "Destroy current location directive.." << std::endl; }

const Array&	Location::getIndexes() const
{ return _index; }

const ErrPage&	Location::getErrPages() const
{ return _error_page; }

const String&	Location::getRoot() const
{ return _root; }

const Redirect&	Location::getRedir() const
{ return _ret; }

const size_t	Location::getMax() const
{ return _max; }

const bool		Location::getAuto() const
{ return _auto; }

const Array&	Location::getAllow() const
{ return _allowed; }

const String&	Location::getCgiPass() const
{ return _fastcgi_pass; }

const String&	Location::getCgiParam() const
{ return _fastcgi_param; }

std::ostream&	operator<<(std::ostream& out, const Location& loc) {
	ErrPage::const_iterator	it;
	Array::const_iterator	it1;

	out << "\n[error_page directive]:\n";
	it = loc.getErrPages().begin();
	for (; it != loc.getErrPages().end(); ++it) {
		out << "code: " << it->first;
		out << "\nuri: " << it->second;
	}
	out << "\n[index directive]:\n";
	it1 = loc.getIndexes().begin();
	for (; it1 != loc.getIndexes().end(); ++it1) {
		out << *it1 << " ";
	}
	out << "\n[allow directive]:\n";
	it1 = loc.getAllow().begin();
	for (; it1 != loc.getAllow().end(); ++it1) {
		out << *it1 << " ";
	}
	out << "\n[client_max_body_size]: ";
	out << loc.getMax();
	out << "\n[root directive]: ";
	out << loc.getRoot();
	out << "\n[autoindex directive]:\n";
	out << loc.getAuto();
	out << "\n[return directive]:\n";
	out << loc.getRedir();
	out << "\n[fastcgi_pass directive]:\n";
	out << loc.getCgiPass();
	out << "\n[fastcgi_param directive]:\n";
	out << loc.getCgiParam();
	return out;
}