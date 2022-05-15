#include "Server.hpp"

Server::Server() : _max(1024)
{ std::cout << "\t[Create Server]" << std::endl; }

Server::~Server() {
	route_t::iterator	it;

	std::cout << "Destroy current server Server..." << std::endl;
	for (it = _routes.begin(); it != _routes.end(); ++it) {
		if (it->second)
			delete it->second; // calls ~Location()
	}
}

const Server::route_t&	Server::get_routes() const
{ return _routes; }

const Server::pages_t&	Server::get_err_pages() const
{ return _error_pages; }

const sockets_t&	Server::get_sockets() const
{ return _sockets; }

const strs_t&	Server::get_names() const
{ return _names; }

size_t		Server::get_max() const
{ return _max; }

void	Server::set_socket(const socket_t& socket)
{ _sockets.push_back(socket); }

void	Server::set_max(const str_t& line)
{ _max = std::atoi(line.c_str()); }

void	Server::set_err_page(const strs_t& line) {
	str_t	value;
	int		key;

	value = line[2];
	key = std::atoi(line[1].c_str());
	_error_pages.insert(std::make_pair(key, value));
}

void	Server::set_route(const str_t& key, Location** loc) {
	std::pair<str_t, Location*>	item;

	item = std::make_pair(key, *loc);
	_routes.insert(item);
}

void	Server::set_names(const strs_t& line) {
	strs_t::const_iterator	it;

	it = line.begin() + 1;
	for (; it != line.end(); ++it) {
		_names.push_back(*it);
	}
}

void	Server::sanitize_sockets() {
	sockets_t::iterator	it1;
	sockets_t::iterator	it2;
	sockets_t::iterator	ite;
	sockets_t			curr;

	curr = get_sockets();
	std::sort(curr.begin(), curr.end());
	it1 = curr.begin();
	ite = curr.end();
	for (; it1 != ite; ++it1) { // remove useless sockets
		if (it1->first == "0.0.0.0") {
			str_t port = it1->second;

			it2 = it1 + 1;
			while (it2 != ite) {
				if (it2->second == port) {
					curr.erase(it2);
					ite = curr.end();
				}
				else
					++it2;
			}
		}
		else
			break ;
	}
}

std::ostream&	operator<<(std::ostream& out, const Server& server) {
	Server::pages_t::const_iterator	it1;
	Server::route_t::const_iterator	it2;
	sockets_t::const_iterator		it3;
	strs_t::const_iterator			it4;

	out << "\t[Error pages]:\n";
	it1 = server.get_err_pages().begin();
	for (; it1 != server.get_err_pages().end(); ++it1) {
		out << "code: " << it1->first;
		out << "\nuri: " << it1->second;
		out << "\n";
	}

	out << "\t[Sockets]:\n";
	it3 = server.get_sockets().begin();
	for (; it3 != server.get_sockets().end(); ++it3) {
		out << it3->first;
		out << ":";
		out << it3->second;
		out << "\n";
	}

	out << "\t[route_t]:\n";
	it2 = server.get_routes().begin();
	for (; it2 != server.get_routes().end(); ++it2) {
		out << "url: " << it2->first;
		out << "\ndirectives:\n" << *(it2->second);
	}

	out << "\t[Names]:\n";
	it4 = server.get_names().begin();
	for (; it4 != server.get_names().end(); ++it4) {
		out << " " << *it4;
	}
	out << "\n";

	out << "\t[client_max_body_size]:\n";
	out << server.get_max();
	out << "\n";
	return out;
}
