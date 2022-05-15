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

/* remove useless sockets */
void	Parser::_smart_map() {
	listenners_t::iterator	it;
	listenners_t::iterator	it1;
	str_t port;

	it = _dumb_map.begin();
	for (; it != _dumb_map.end(); ++it) {
		if (it->first.first == "0.0.0.0") {
			port = it->first.second;
			it1 = it;
			++it1;
			for (; it1 != _dumb_map.end(); ++it1) {
				if (it1->first.second == port)
					_dumb_map.erase(it1);
			}
		}
		else
			break ;
	}
}

void	Parser::_fill_map(int flag) {
	sockets_t::const_iterator	it;
	listenners_t::iterator		res;
	servers_t		servers;

	if (flag) {
		_curr_serv->sanitize_sockets();
		it = _curr_serv->get_sockets().begin();
		for (; it != _curr_serv->get_sockets().end(); ++it) {
			res = _dumb_map.find(*it);
			if (res != _dumb_map.end()) // socket found
				res->second.push_back(_curr_serv);
			else {
				res = _dumb_map.find(std::make_pair("0.0.0.0", (*it).second));
				if (res != _dumb_map.end()) // socket listenning on * address found
					res->second.push_back(_curr_serv);
				else { // new socket
					servers.clear();
					servers.push_back(_curr_serv);
					_dumb_map.insert(std::make_pair(*it, servers));
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
	_smart_map();
	return SUCCESS;
}
