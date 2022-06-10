#include "Response.hpp"
#include "Cgi.hpp"

static str_t	get_path(const str_t& uri)
{ return uri.substr(0, uri.find_last_of('?')); }

Response::Response(const int fd, const Server* serv, const Request* req, str_t& raw) :  _buffer(raw), _socket(fd), _server(serv), _request(req)
{ std::cout << "Create Response" << std::endl; }

Response::~Response()
{ std::cout << "Destroy Response" << std::endl; }

const Server*	Response::get_server() const
{ return _server; }

bool	Response::_method_allowed(const Location* uri_loc, const str_t& method) const {
	strs_t::const_iterator	it;

	it = uri_loc->get_allowed().begin();
	for (; it != uri_loc->get_allowed().end(); ++it) {
		if (*it == method) {
			return true;
		}
	}
	return false;
}

str_t	Response::_fetch_mime(int code) {
	Request::fields_t::const_iterator	it;
	str_t	path;
	size_t	pos;

	path = get_path(_request->get_rl()[1]);
	pos = path.find_last_of('.');
	if (code >= 400 || pos == std::string::npos)
		return "text/html";
	if ((it = mime_g.find(path.substr(pos + 1))) == mime_g.end())
		return "text/html";
	return it->second;
}

bool	Response::_extract_content(const str_t* path) {
	std::ifstream	infile;
	size_t	rlen;
	char*	buff;

	infile.open(path->c_str());
	if (!infile.is_open())
		return FAILURE;
	infile.seekg(0, infile.end);
	rlen = infile.tellg();
	infile.seekg(0, infile.beg);
	buff = new char[rlen];
	infile.readsome(buff, rlen);
	_buffer.append(buff, rlen);
	infile.close();
	delete [] buff;
	return SUCCESS;
}

bool	Response::_extract_directory(const str_t& route, const str_t& subroute) {
	DIR *dp;
	struct dirent *dirp;

	std::cout << "route: " << route << "\nsubroute: " << subroute << "\n";
	if ((dp = opendir(route.c_str())) == NULL)
		return FAILURE;
	_buffer += "<html>\n<head><title>Index of " + route;
	_buffer += "</title></head>\n<body>\n<h1>Index of " + route;
	_buffer += "</h1>\n<hr><pre>\n<a href=\"../\">../</a>\n";
	while ((dirp = readdir(dp)) != NULL) {
		if (dirp->d_name[0] != '.') {
			_buffer += "<a href=\"";
			_buffer += subroute + dirp->d_name;
			_buffer += "\">";
			_buffer += dirp->d_name;
			_buffer += "</a>\n";
		}
	}
	_buffer += "</pre><hr>\r\n</body>\n</html>\n";
	closedir(dp);
	return SUCCESS;
}

void	Response::_set_header(int code, const str_t* redir) {
	str_t	data;
	str_t	time;

	std::time_t fetch_time = std::time(NULL);
	time = std::asctime(std::localtime(&fetch_time)); //to do: change format
	*time.rbegin() = CR;
	data += "HTTP/1.1 " + _itoa(code) + " " + code_g[code] + CRLF;
	data += "Server: " SERVER CRLF;
	data += "Date: " + time;
	data += "\nContent-Type: " + _fetch_mime(code) + CRLF;
	data += "Content-Length: " + _itoa(_buffer.size()) + CRLF;
	if (redir)
		data += "Location: " + (*redir) + CRLF;
	if (redir || code == 200 || code == ERR_403 || code == ERR_405)
		data += "Connection: Keep-alive" CRLF CRLF;
	else
		data += "Connection: close" CRLF CRLF;
	data += _buffer;
	_buffer = data;
}

inline bool ends_with(const str_t& value,const str_t& ending)
{
	if (ending.size() > value.size()) return false;
		return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
}

void	Response::process_get(const Location* uri_loc, const str_t& route) {
	bool	autoindex;
	str_t	subroute;

	subroute = route.substr(uri_loc->get_root().size() - 1);
	autoindex = uri_loc->get_autoindex();
	if (*route.rbegin() == '/') {
		if (autoindex == false)
			error_response(403);
		else if (_extract_directory(route, subroute) == FAILURE)
			error_response(500);
		else
			_set_header(200);
	}
	else if (!uri_loc->get_cgi().empty() && (ends_with(route, ".php") || ends_with(route, ".py"))) {
		return _process_cgi(uri_loc, route);
	}
	else if (_extract_content(&route) == FAILURE)
		error_response(500);
	else
		_set_header(200);
	return ;
}

static int	delete_file(const char* path, const struct stat* sb, int flag, struct FTW* buf) {
	printf("%-3s %2d ",
		(flag == FTW_D) ?   "d"   : (flag == FTW_DNR) ? "dnr" :
		(flag == FTW_DP) ?  "dp"  : (flag == FTW_F) ?   "f" :
		(flag == FTW_NS) ?  "ns"  : (flag == FTW_SL) ?  "sl" :
		(flag == FTW_SLN) ? "sln" : "???",
		buf->level);

	if (flag == FTW_NS)
		printf("-------");
	else
		printf("%7jd", (intmax_t) sb->st_size);
	printf("   %-40s %d %s\n", path, buf->base, path + buf->base);
	return remove(path);
}

void	Response::process_delete(const str_t& path) {
	// to do: delete in DELETE folder only else error 501
	if (*path.rbegin() == '/') {
		if (nftw(path.c_str(), delete_file, 20, FTW_DEPTH | FTW_PHYS) < 0)
			error_response(500);
		else
			_set_header(200);
	}
	else if (remove(path.c_str()) != SUCCESS)
		error_response(500);
	else
		_set_header(200);
	return ;
}

void	Response::_process_cgi(const Location* uri_loc, const str_t& route) {
	str_t	bin = uri_loc->get_cgi();
	*route.rbegin() == 'p' ? bin += "php-cgi" : bin += "python3";
	Cgi cgi(*_request, bin);

	cgi.build_env(route, _socket);
	if (cgi.exec_cgi(route) || cgi.treat_cgi_output(_buffer))
		error_response(500);
}

void	Response::process_post(const Location* uri_loc, const str_t& route) {
	if (uri_loc->get_cgi().empty() && !(ends_with(route, ".php") || ends_with(route, ".py")))
		return error_response(501);
	_process_cgi(uri_loc, route);
}

void	Response::error_response(int code) {
	Server::pages_t::const_iterator	it;

	_buffer.clear();
	if (_server) {
		it = _server->get_err_pages().find(code);
		if (it != _server->get_err_pages().end()) {
			if (_extract_content(&it->second) == FAILURE)
				_buffer += page_g[ERR_500];
		}
	}
	if (_buffer.empty())
		_buffer += page_g[code];
	if (_request->get_rl().size() == 3 && _request->get_rl()[2].substr(0, 5) == "HTTP/")
		_set_header(code);
}

/*
** Aim: search and expand route requested
** return server requested location block
*/
const Location*	Response::construct_route(str_t& route) const {
	Server::route_t::const_iterator it;
	str_t	path;
	size_t	pos;

	path = get_path(_request->get_rl()[1]);
	pos = std::string::npos;
	do {
		pos = path.find_last_of('/', pos);
		it = get_server()->get_routes().find(path.substr(0, pos + 1));
		if (it != get_server()->get_routes().end()) {
			route = it->second->get_root() + path.substr(pos + 1); {
			std::cout << "ROUTE: " << route << '\n';
			return it->second;
			}
		}
		pos ? pos -= 1 : 0;
	} while (pos);
	return NULL;
}

void	Response::process_method(const Location* uri_loc, const str_t& route) {
	str_t	method;
	bool	redir;

	method = _request->get_rl()[0];
	redir = uri_loc->get_redir().first != -1;
	if (_method_allowed(uri_loc, method) == false)
		error_response(405);
	else if (redir == true) {
		if (_extract_content(&uri_loc->get_redir().second) == FAILURE)
			error_response(404);
		else
			_set_header(301, &uri_loc->get_redir().second);
	}
	else if (method == "POST")
		process_post(uri_loc, route);
	else if (method == "GET")
		process_get(uri_loc, route);
	else if (method == "DELETE")
		process_delete(route);
}
