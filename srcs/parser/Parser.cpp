#include "Parser.hpp"

Parser::Parser(const char* file) : _error(0) {
	this->open_file(file);
	this->loop(0);
	if (_error)
		throw "Wrong config file!";
	this->open_file(file);
	this->loop(1);
}

Parser::~Parser()
{}

const Parser::Listenning&	Parser::getMap() const
{ return _map; }

int		Parser::getError() const
{ return _error; }

void	Parser::open_file(const char* file) {
	_in.open(file);
	if (!_in.is_open())
		_error = 1;
}

/* split read buffer in strings then check for empty line */
bool	Parser::empty_line() {
	std::stringstream	tmp(_buffer);
	String	elem;

	_line.clear();
	while (tmp >> elem) {
#if DEBUG
		printf("[%s] ", elem.c_str());
#endif
		_line.push_back(elem);
	}
#if DEBUG
	printf("\n");
#endif
	if (_line.size() == 0)
		return 1;
	return 0;
}

void	Parser::fill_map(int flag) {
	Config::Sockets::const_iterator	it;
	static Event::Servers			servers;
	Listenning::iterator			res;
	Config::Sockets					curr = _curr_conf->getSockets();

	if (flag && !_error) {
		if (curr.empty())
			_curr_conf->setSocket(std::make_pair("127.0.0.1", "80"));
		std::sort(curr.begin(), curr.end());
		_curr_serv = new Server(_curr_conf);
		for (it = curr.begin(); it != curr.end(); ++it) {
			res = _map.find(*it);
			if (res != _map.end())
				res->second.push_back(_curr_serv);
			else {
				res = _map.find(std::make_pair("0.0.0.0", (*it).second));
				if (res != _map.end())
					res->second.push_back(_curr_serv);
				else {
					servers.clear();
					servers.push_back(_curr_serv);
					_map.insert(std::make_pair(*it, servers));
				}
			}	
		}
	}
	_tmp.clear();
}

void	Parser::loop(int flag) {
	while (!_error && std::getline(_in, _buffer)) {
		if (empty_line())
			continue ;
		else if (_line[0] == "}" && _line.size() != 1)
			_error = 3;
		else if (_line[0] == "server" && _line.size() == 2 && _line[1] == "{") {
			if (flag)
				_curr_conf = new Config();
			while (!_error && std::getline(_in, _buffer)) {
				if (empty_line())
					continue ;
				else if (_line[0] == "}" && _line.size() == 1)
					break ;
				else if (_line[0] == "}" && _line.size() != 1)
					_error = 3;
				else if (wrong_sdirective(flag))
					_error = 4;
			}
			fill_map(flag);
		}
		else
			_error = 2;
	}
	_in.close();
}
