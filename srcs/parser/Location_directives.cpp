#include "Parser.hpp"

bool	Parser::allow_directive(int flag) {
	if (!flag) {
		if (_line.size() == 1 || *_line.rbegin() != ";")
			return FAILURE;
	}
	else
		_curr_loc->setMethod(_line);
	return SUCCESS;
}

bool	Parser::cgi_directive(int flag) {
	if (!flag) {
		if (_line.size() != 3 || *_line.rbegin() != ";")
			return FAILURE;
	}
	else
		_curr_loc->setCgi(_line[1]);
	return SUCCESS;
}

bool	Parser::return_directive(int flag) {
	int	i;

	if (!flag) {
		i = 0;
		if (_line.size() != 4 || *_line.rbegin() != ";")
			return FAILURE;
		if (_line[1] != "301")
			return FAILURE;
	}
	else
		_curr_loc->setRedirection(_line);
	return SUCCESS;
}

bool	Parser::autoIndex_directive(int flag) {
	if (!flag) {
		if (_line.size() != 3 || *_line.rbegin() != ";")
			return FAILURE;
		else if (_line[1] != "on" && _line[1] != "off")
			return FAILURE;
	}
	else
		_curr_loc->setAutoIndex(_line[1]);
	return SUCCESS;
}

bool	Parser::root_directive(int flag) {
	if (!flag) {
		if (_line.size() != 3 || *_line.rbegin() != ";")
			return FAILURE;
	}
	else
		_curr_loc->setRoot(_line[1]);
	return SUCCESS;
}

bool	Parser::wrong_ldirective(int flag) {
	if (_line[0] == "allow")
		return allow_directive(flag);
	else if (_line[0] == "cgi")
		return cgi_directive(flag);
	else if (_line[0] == "return")
		return return_directive(flag);
	else if (_line[0] == "autoindex")
		return autoIndex_directive(flag);
	else if (_line[0] == "root")
		return root_directive(flag);
	return FAILURE;
}

bool	Parser::location_directive(int flag) {
	String	key;

	if (_line.size() != 3 || *_line.rbegin() != "{")
		return FAILURE;
	if (flag) {
		key = _line[1];
		_curr_loc = new Location();
	}
	while (std::getline(_in, _buffer)) {
		if (_empty_line())
			continue ;
		if (_line[0] == "}" && _line.size() == 1)
			break ;
		else if (_line[0] == "}" && _line.size() != 1)
			return FAILURE;
		else if (wrong_ldirective(flag))
			return FAILURE;
	}
	if (flag)
		_curr_conf->setRoutes(key, &_curr_loc);
	return SUCCESS;
}
