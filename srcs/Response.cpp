#include "Response.hpp"

static str_t	_get_path(const str_t& uri)
{ return uri.substr(0, uri.find_last_of('?')); }

static str_t	_fetch_mime(const str_t& route, int code) {
	Request::fields_t::const_iterator	it;
	str_t	path;
	size_t	pos;

	if (code == 200) {
		path = _get_path(route);
		pos = path.find_last_of('.');
		it = mime_g.find(path.substr(pos + 1));
		if (it != mime_g.end())
			return it->second;
	}
	return "text/html";
}

static int	_translate(int hex) {
	if (hex == ERR_400)
		return 400;
	if (hex == ERR_403)
		return 403;
	if (hex == ERR_404)
		return 404;
	if (hex == ERR_405)
		return 405;
	if (hex == ERR_414)
		return 414;
	if (hex == ERR_500)
		return 500;
	if (hex == ERR_501)
		return 501;
	return 505;
}

Response::Response(str_t& raw) : _buffer(raw)
{ std::cout << "Create Response" << std::endl; }

Response::~Response()
{ std::cout << "Destroy Response" << std::endl; }

void	Response::set_server(const Server* serv)
{ _server = serv; }

void	Response::set_request(const Request* req)
{ _request = req; }

bool	Response::extract_content(const str_t* path) {
	std::ifstream	infile;
	size_t	rlen;
	char*	buff;

	infile.open(path->c_str());
	if (infile.is_open()) {
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
	return FAILURE;
}

bool	Response::extract_directory(const str_t& route, const str_t& subroute) {
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

void	Response::set_header(int code, const str_t& path) {
	str_t	data;
	str_t	time;

	std::time_t fetch_time = std::time(NULL);
	time = std::asctime(std::localtime(&fetch_time)); //to do: change format
	*time.rbegin() = CR;
	data += "HTTP/1.1 " + _itoa(code) + " " + code_g[code] + CRLF;
	data += "Server: " SERVER CRLF;
	data += "Date: " + time;
	data += "\nContent-Type: " + _fetch_mime(path, code) + CRLF;
	data += "Content-Length: " + _itoa(_buffer.size()) + CRLF;
	if (code == 301)
		data += "Location: " + path + CRLF;
	if (code == 301 || code == 200 || code == 403 || code == 405)
		data += "Connection: Keep-alive" CRLF CRLF;
	else
		data += "Connection: close" CRLF CRLF;
	data += _buffer;
	_buffer = data;
}

void	Response::error_response(int code) {
	Server::pages_t::const_iterator	it;
	strs_t	rline;

	_buffer.clear();
	code = _translate(code);
	rline = _request->get_rl();
	if (_server) {
		it = _server->get_err_pages().find(code);
		if (it != _server->get_err_pages().end()) {
			if (extract_content(&it->second) == FAILURE)
				_buffer += page_g[500];
		}
	}
	if (_buffer.empty())
		_buffer += page_g[code];
	if (rline.size() == 3 && rline[2].substr(0, 5) == "HTTP/")
		set_header(code, _get_path(rline[1]));
}

/*
** Aim: search and expand route requested
** fill server requested location block
*/
void	Response::construct_route(const Location** uri_loc, str_t& route) const {
	Server::route_t::const_iterator it;
	str_t	url;
	size_t	pos;

	pos = std::string::npos;
	url = _get_path(_request->get_rl()[1]);
	do {
		pos = url.find_last_of('/', pos);
		it = _server->get_routes().find(url.substr(0, pos + 1));
		if (it != _server->get_routes().end()) {
			route = it->second->get_root() + url.substr(pos + 1);
			*uri_loc = it->second;
			break ;
		}
		pos ? pos -= 1 : 0;
	} while (pos);
	return ;
}
