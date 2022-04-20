#include "Parser.hpp"

bool	Parser::wrong_sdirective(int flag) {
	if (_line[0] == "listen")
		return listen_directive(flag);
	else if (_line[0] == "error_page")
		return errPage_directive(flag);
	else if (_line[0] == "max_client_body_size")
		return clienSize_directive(flag);
	else if (_line[0] == "server_name")
		return names_directive(flag);
	else if (_line[0] == "location")
		return location_directive(flag);
	return FAILURE;
}

bool	Parser::names_directive(int flag) {
	if (!flag) {
		if (_line.size() < 3 || *_line.rbegin() != ";")
			return FAILURE;
	}
	else
		_curr_conf->setNames(_line);
	return SUCCESS;
}

bool	Parser::clienSize_directive(int flag) {
	char	*end;

	if (!flag && (_line.size() != 3 || *_line.rbegin() != ";"))
		return FAILURE;
	std::strtol(_line[1].c_str(), &end, 10);
	if (!flag && (*end || _line[1].size() > 4))
		return FAILURE;
	if (flag)
		_curr_conf->setMax(_line[1]);
	return SUCCESS;
}

/* tuple host:port mandatory */
bool	Parser::listen_directive(int flag) {
	Config::Sockets::const_iterator	it;
	String	port;
	String	host;
	size_t	pos;
	char*	end;

	if (!flag && (_line.size() != 3 || *_line.rbegin() != ";"))
		return FAILURE;
	pos = _line[1].find(':');
	if (pos == std::string::npos)
		return FAILURE;
	host = _line[1].substr(0, pos);
	port = _line[1].substr(pos + 1);
	if (host == "localhost")
		host = "127.0.0.1";
	else if (host == "*")
		host = "0.0.0.0";
	std::strtol(port.c_str(), &end, 10);
	if (!flag) {
		it = _tmp.begin();
		if (port.size() > 5 || *end)
			return FAILURE;
		for (; it != _tmp.end(); ++it) {
			if (it->first == host && it->second == port)
				return FAILURE;
		}
		_tmp.push_back(std::make_pair(host, port));
	}
	else
		(_curr_conf)->setSocket(std::make_pair(host, port));
	return SUCCESS;
}

bool	Parser::errPage_directive(int flag) {
	const char*	err[] = {"400", "403", "404", "405", "500", "501", "505", NULL};
	int	i;

	if (!flag) {
		i = 0;
		if (_line.size() != 4 || *_line.rbegin() != ";")
			return FAILURE;
		while (err[i] && _line[1] != String(err[i]))
			++i;
		if (!err[i])
			return FAILURE;
	}
	else
		_curr_conf->setErrorPage(_line);
	return SUCCESS;
}
