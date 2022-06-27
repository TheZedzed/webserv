#include "Response.hpp"
#include "Cgi.hpp"

static int	_delete_file(const char* path, const struct stat* sb, int flag, struct FTW* buf) {
	(void)flag;
	(void)sb;
	(void)buf;
	return remove(path);
}

static bool	_test_path(str_t& path, int& state) {
	struct stat	st;

	if (stat(path.c_str(), &st) == -1) {
		if (errno == EACCES)
			state |= (ERROR | ERR_403);
		else if (errno == ENOENT)
			state |= (ERROR | ERR_404);
		else if (errno == ENAMETOOLONG)
			state |= (ERROR | ERR_414);
		else
			state |= (ERROR | ERR_500);
		return FAILURE;
	}
	if ((st.st_mode & S_IFMT) == S_IFDIR && *path.rbegin() != '/')
		path += "/";
	return SUCCESS;
}

Response::Response(const Server* serv, const Request* req) : _server(serv), _request(req), _location(NULL)
{ std::cout << "Create Response" << std::endl; }

Response::~Response()
{ std::cout << "Destroy Response" << std::endl; }

str_t	Response::extract_content(int& state) {
	std::ifstream	infile;
	char*	buff;
	size_t	rlen;
	str_t	res;

	if (_test_path(_path, state) == FAILURE)
		res = page_g[_translate(state)];
	else {
		infile.open(_path.c_str());
		infile.seekg(0, infile.end);
		rlen = infile.tellg();
		infile.seekg(0, infile.beg);
		buff = new char[rlen];
		infile.readsome(buff, rlen);
		infile.close();
		res.append(buff, rlen);
		delete [] buff;
	}
	return res;
}

str_t	Response::extract_directory(int& state) {
	struct dirent *dirp;
	str_t	subroute;
	str_t	res;
	DIR *dp;

	subroute = _path.substr(_path.size() - _request->get_rl()[1].size());
	if ((dp = opendir(_path.c_str()))) {
		res += "<html>\n<head><title>Index of " + subroute;
		res += "</title></head>\n<body>\n<h1>Index of " + subroute;
		res += "</h1>\n<hr><pre>\n<a href=\"../\">../</a>\n";
		while ((dirp = readdir(dp)) != NULL) {
			if (dirp->d_name[0] != '.') {
				res += "<a href=\"";
				res += subroute + dirp->d_name;
				res += "\">";
				res += dirp->d_name;
				res += "</a>\n";
			}
		}
		res += "</pre><hr>\r\n</body>\n</html>\n";
		closedir(dp);
		return res;
	}
	state = (RESPONSE | ERROR | ERR_500);
	return error_response(state);
}

str_t	Response::error_response(int& state) {
	Server::pages_t::const_iterator	it;
	str_t	res;

	it = _server->get_err_pages().find(_translate(state));
	if (it != _server->get_err_pages().end()) {
		_path = it->second;
		res += extract_content(state);
	}
	else
		res += page_g[_translate(state)];
	return res;
}

/*
** Aim: search and expand route requested
** fill server requested location block
*/
bool	Response::construct_path(int& state) {
	Server::route_t::const_iterator it;
	char	buf[4097];
	str_t	path;
	str_t	uri;
	size_t	pos;

	pos = std::string::npos;
	uri = _request->get_rl()[1];
	path = uri.substr(0, uri.find_last_of('?'));
	do {
		pos = path.find_last_of('/', pos);
		it = _server->get_routes().find(path.substr(0, pos + 1));
		if (it != _server->get_routes().end()) {
			bzero(buf, 4097);
			_path = getcwd(buf, 4096);
			_path += it->second->get_root() + path.substr(pos + 1);
			_location = it->second;
			break ;
		}
		pos ? pos -= 1 : 0;
	} while (pos);
	return _test_path(_path, state);
}

str_t	Response::process_get(int& state) {
	bool	autoindex = _location->get_autoindex();
	bool	directory = *_path.rbegin() == '/';
	const str_t	index = _location->get_index();;
	str_t	res;

	if (directory) {
		_path += index;
		res = extract_content(state);
		if (state & ERROR) {
			if (autoindex) {
				res.clear();
				state = (RESPONSE);
				_path = _path.substr(0, _path.size() - index.size());
				res = extract_directory(state);
			}
			else {
				state |= (ERROR | ERR_403);
				return error_response(state);
			}
		}
	}
	else
		res = extract_content(state);
	if (state & ERROR)
		return error_response(state);
	state = (RESPONSE | ERR_200);
	return res;
}

void	Response::process_cgi(int socket, int& state, str_t& raw_data) {
	str_t	bin;

	bin = _location->get_cgi();
	*_path.rbegin() == 'p' ? bin += "php-cgi" : bin += "python3";
	Cgi cgi(*_request, bin);
	cgi.build_env(_path, socket);
	if (cgi.exec_cgi(_path) || cgi.treat_cgi_output(raw_data)) {
		state |= (ERROR | ERR_500);
		raw_data = error_response(state);
	}
	else
		state = (RESPONSE | ERR_200);
	return ;
}

str_t	Response::process_delete(int& state) {
	// to do: delete in DELETE folder only else error 501
	if (_path.find("/delete/") == std::string::npos) {
		state |= (ERROR | ERR_403);
		return error_response(state);
	}
	if (*_path.rbegin() == '/') {
		if (nftw(_path.c_str(), _delete_file, 20, FTW_DEPTH | FTW_PHYS) < 0) {
			state |= (ERROR | ERR_500);
			return error_response(state);
		}
	}
	else if (remove(_path.c_str()) != SUCCESS) {
		state |= (ERROR | ERR_500);
		return error_response(state);
	}
	state = (RESPONSE | ERR_200);
	return str_t();
}

str_t	Response::process_redir(int& state) {
	str_t	redirection;
	str_t	res;

	redirection = _location->get_redir().second;
	_path = redirection;
	res += extract_content(state);
	if (!(state & ERROR))
		state = (RESPONSE | ERR_301);
	return res;
}
