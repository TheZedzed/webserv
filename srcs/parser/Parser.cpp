#include "Parser.hpp"

Parser::Parser(const char* file) {
	std::ifstream	infile;

	try {
		infile.open(file);
		loop(infile, 0);
		infile.close();
		infile.open(file);
		loop(infile, 1);
		infile.close();
	}
	catch (...) {
		std::cerr << CONF_ERR << std::endl;
		if (infile.is_open())
			infile.close();
		throw ;
	}
}

const Parser::listenners_t&	Parser::getMap() const
{ return _dumb_map; }

/* remove useless sockets */
void	Parser::_smart_map() {

}

void	Parser::_fill_map(int flag) {
	sockets_t::const_iterator	it;
	listenners_t::iterator		res;
	servers_t		servers;

	if (flag) {
		_curr_serv->sanitize_sockets();
		it = _curr_serv->getSockets().begin();
		for (; it != _curr_serv->getSockets().end(); ++it) {
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
	if (std::getline(in, buffer)) { // split line
		tmp.str(buffer);
		while (tmp >> elem)
			_line.push_back(elem);
		if (_line.size() == 0 || _end_of_block()) // empty line or "}"
			return _getline(in, buffer);
	}
	return false;
}

void	Parser::loop(stream_t& in, bool flag) {
	str_t	buffer;

	if (!in.is_open())
		throw CONF_ERR;
	while (_getline(in, buffer)) {
		if (_server_block()) {
			if (flag)
				_curr_serv = new Server();
			while (_getline(in, buffer)) {
				if (server_directive(in, flag) == FAILURE)
					throw CONF_ERR;
			}
			if (_curr_serv->getSockets().empty())
				throw CONF_ERR;
			_fill_map(flag);
			continue ;
		}
		throw CONF_ERR;
	}
	_smart_map();
}
