#include "Parser.hpp"

bool	Parser::wrong_ldirective(int flag) {
	if (_line[0] == "allow")
		return allow_directive(flag);
	else if (_line[0] == "cgi")
		return cgi_directive(flag);
	else if (_line[0] == "return")
		return return_directive(flag);
	else if (_line[0] == "autoindex")
		return autoindex_directive(flag);
	else if (_line[0] == "root")
		return root_directive(flag);
	return FAILURE;
}

bool	Parser::allow_directive(int flag) {
	if (!flag) {
		if (_line.size() == 1 || *_line.rbegin() != ";")
			return FAILURE;
	}
	else
		_curr_loc->set_method(_line);
	return SUCCESS;
}

bool	Parser::cgi_directive(int flag) {
	if (!flag) {
		if (_line.size() != 3 || *_line.rbegin() != ";")
			return FAILURE;
	}
	else
		_curr_loc->set_cgi(_line[1]);
	return SUCCESS;
}

bool	Parser::return_directive(int flag) {
	if (!flag) {
		if (_line.size() != 4 || *_line.rbegin() != ";")
			return FAILURE;
		if (_line[1] != "301")
			return FAILURE;
	}
	else
		_curr_loc->set_redirection(_line);
	return SUCCESS;
}

bool	Parser::autoindex_directive(int flag) {
	if (!flag) {
		if (_line.size() != 3 || *_line.rbegin() != ";")
			return FAILURE;
		else if (_line[1] != "on" && _line[1] != "off")
			return FAILURE;
	}
	else
		_curr_loc->set_autoindex(_line[1]);
	return SUCCESS;
}

bool	Parser::root_directive(int flag) {
	if (!flag) {
		if (_line.size() != 3 || *_line.rbegin() != ";")
			return FAILURE;
	}
	else
		_curr_loc->set_root(_line[1]);
	return SUCCESS;
}
