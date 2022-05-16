#include "Parser.hpp"

static const char*	err[] = {"400", "403", "404", "405", "413", "414", "500", "501", "505", NULL};

bool	Parser::server_directive(stream_t& in, int flag) {
	if (_line[0] == "listen")
		return listen_directive(flag);
	else if (_line[0] == "error_page")
		return err_page_directive(flag);
	else if (_line[0] == "max_client_body_size")
		return client_size_directive(flag);
	else if (_line[0] == "server_name")
		return names_directive(flag);
	else if (_line[0] == "location")
		return location_directive(in, flag);
	return FAILURE;
}

bool	Parser::names_directive(int flag) {
	if (!flag) {
		if (_line.size() < 3 || *_line.rbegin() != ";")
			return FAILURE;
	}
	else
		_curr_serv->set_names(_line);
	return SUCCESS;
}

bool	Parser::client_size_directive(int flag) {
	char	*end;

	if (!flag && (_line.size() != 3 || *_line.rbegin() != ";"))
		return FAILURE;
	std::strtol(_line[1].c_str(), &end, 10);
	if (!flag && (*end || _line[1].size() > 4))
		return FAILURE;
	if (flag)
		_curr_serv->set_max(_line[1]);
	return SUCCESS;
}

socket_t	Parser::_lil_dns(void) {
	str_t	host;
	str_t	port;
	size_t	pos;

	pos = _line[1].find(':');
	if (pos == std::string::npos)
		return std::make_pair("", "");
	host = _line[1].substr(0, pos);
	port = _line[1].substr(pos + 1);
	if (host == "localhost")
		return std::make_pair("127.0.0.1", port);
	else if (host == "*" || host == "0.0.0.0")
		return std::make_pair("0.0.0.0", port);
	return std::make_pair("", "");
}

/*
** tuple host:port
** valid hosts: loacalhost or 127.0.0.1 or 0.0.0.0
*/
bool	Parser::listen_directive(int flag) {
	sockets_t::iterator	ite;
	sockets_t::iterator	it;
	socket_t	val;
	char*	end;

	if (!flag && (_line.size() != 3 || *_line.rbegin() != ";"))
		return FAILURE;
	val = _lil_dns();
	if (!flag && val.first == "")
		return FAILURE;
	if (!flag) {
		it = _dumb_tmp.begin();
		ite = _dumb_tmp.end();
		std::strtol(val.second.c_str(), &end, 10);
		if (val.second.size() > 5 || *end)
			return FAILURE;
		else if (std::find(it, ite, val) != ite)
			return FAILURE;
		_dumb_tmp.push_back(val);
	}
	else
		(_curr_serv)->set_socket(val);
	return SUCCESS;
}

bool	Parser::err_page_directive(int flag) {
	int	i;

	if (!flag) {
		i = 0;
		if (_line.size() != 4 || *_line.rbegin() != ";")
			return FAILURE;
		while (err[i] && _line[1] != str_t(err[i]))
			++i;
		if (!err[i])
			return FAILURE;
	}
	else
		_curr_serv->set_err_page(_line);
	return SUCCESS;
}
