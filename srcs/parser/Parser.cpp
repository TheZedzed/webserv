#include "Parser.hpp"

Parser::Parser(const char* file) {
	std::ifstream	infile;

	infile.open(file);
	if (!infile.is_open() || loop(infile, 0) == FAILURE) {
		if (infile.is_open())
			infile.close();
		throw std::runtime_error("Error parsing conf file!");
	}
	infile.close();
	infile.open(file);
	loop(infile, 1);

	listenners_t::const_iterator	it;
	servers_t::const_iterator	it1;
	int	i;

	i = -1;
	it = _dumb_map.begin();
	for (; it != _dumb_map.end(); ++it) {
		it1 = it->second.begin();
		std::cout << "Server id: " << ++i << std::endl;
		for (; it1 != it->second.end(); ++it1) {
			std::cout << **it1;
		}
	}
}

Parser::~Parser() {
	listenners_t::iterator	it1;
	servers_t::iterator		it2;

	for (it1 = _dumb_map.begin(); it1 != _dumb_map.end(); ++it1) {
		it2 = it1->second.begin();
		for (; it2 != it1->second.end(); ++it2) {
			delete *it2;
		}
	}
}

const Parser::listenners_t&	Parser::get_map() const
{ return _dumb_map; }

void	Parser::_new_node(const socket_t socket, Server* server) {
	servers_t	servers;

	servers.push_back(server);
	_dumb_map.insert(std::make_pair(socket, servers));
	servers.clear();
}

void	Parser::_insert_any(const socket_t socket, Server* server) {
	listenners_t::iterator	it2;
	listenners_t::iterator		res;
	servers_t	servers;
	bool	found;

	found =false;
	it2 = _dumb_map.begin();
	res = _dumb_map.find(socket);
	if (res != _dumb_map.end())
		res->second.push_back(server);
	else {
		for(; it2 != _dumb_map.end(); ++it2) {
			if (it2->first.second == socket.second) {
				servers = it2->second;
				servers.push_back(server);
				_dumb_map.insert(std::make_pair(socket, servers));
				_dumb_map.erase(it2);
				servers.clear();
				found = true;
			}
		}
		if (found == false)
			_new_node(socket, server);
	}
}

void	Parser::_fill_map(int flag) {
	sockets_t::const_iterator	it;
	listenners_t::iterator		res;

	if (flag) {
		_curr_serv->sanitize_sockets();
		it = _curr_serv->get_sockets().begin();
		for (; it != _curr_serv->get_sockets().end(); ++it) {
			if (it->first == "0.0.0.0")
				_insert_any(*it, _curr_serv);
			else { // 127.0.0.1
				res = _dumb_map.find(*it);
				if (res != _dumb_map.end())
					res->second.push_back(_curr_serv);
				else {
					res = _dumb_map.find(std::make_pair("0.0.0.0", (*it).second));
					if (res != _dumb_map.end())
						res->second.push_back(_curr_serv);
					else
						_new_node(*it, _curr_serv);
				}
			}
		}
	}
	_dumb_tmp.clear();
}

bool	Parser::_end_of_block()
{ return (_line.size() == 1 && _line[0] == "}"); }

bool	Parser::_server_block()
{ return (_line.size() == 2 && _line[0] == "server" &&  _line[1] == "{"); }

bool	Parser::_getline(stream_t& in, str_t& buffer) {
	std::stringstream	tmp;
	str_t	elem;

	_line.clear();
	if (std::getline(in, buffer)) {
		tmp.str(buffer);
		while (tmp >> elem)
			_line.push_back(elem);
		return true;
	}
	return false;
}

bool	Parser::loop(stream_t& in, bool flag) {
	str_t	buffer;

	while (_getline(in, buffer)) {
		if (_server_block()) {
			if (flag)
				_curr_serv = new Server();
			while (_getline(in, buffer) && !_end_of_block()) {
				if (_line.size() == 0)
					continue ;
				if (server_directive(in, flag) == FAILURE)
					return FAILURE;
			}
			if (flag && _curr_serv->get_sockets().empty())
				_curr_serv->set_socket(std::make_pair("0.0.0.0", "8080"));
			_fill_map(flag);
			continue ;
		}
		else if (_line.size() == 0 || _end_of_block())
			continue ;
		return FAILURE;
	}
	return SUCCESS;
}
