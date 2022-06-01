#include "Response.hpp"
#include <dirent.h>
#include <ctime>

Response::Response(const Server* serv, const Request* req, str_t& raw) :  _buffer(raw), _server(serv), _request(req)
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

bool	Response::_extract_content(const str_t* path) {
	std::ifstream	infile;
	str_t	line;

	infile.open(path->c_str());
	if (!infile.is_open()) // error openning file
		return FAILURE;
	while (std::getline(infile, line))
		_buffer.append(line);
	infile.close();
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
	time = std::asctime(std::localtime(&fetch_time));
	*time.rbegin() = CR;
	data += "HTTP/1.1 " + _itoa(code) + " " + code_g[code] + CRLF;
	data += "Server: " SERVER CRLF;
	data += "Date: " + time;
	data += "\nContent-Type: text/html" CRLF; // mimes_type
	data += "Content-Length: " + _itoa(_buffer.size()) + CRLF;
	if (redir)
		data += "Location: " + (*redir) + CRLF;
	// to do : timer
	//if (redir || code == 200 || code == ERR_403 || code == ERR_405)
	//	data += "Connection: Keep-alive" CRLF;
	//else
	data += "Connection: close" CRLF CRLF;
	data += _buffer;
	_buffer = data;
}

void	Response::process_get(const Location* uri_loc, const str_t& route) {
	bool	autoindex;
	str_t	subroute;

	subroute = route.substr(uri_loc->get_root().size() - 1);
	autoindex = uri_loc->get_autoindex();
	if (*route.rbegin() == '/') {
		if (autoindex == false)
			return error_response(403);
		if (_extract_directory(route, subroute) == FAILURE)
			return error_response(500);
	}
	else if (_extract_content(&route) == FAILURE)
		return error_response(500);
	_set_header(200);
}

void	Response::process_delete(const Location* uri_loc, const str_t& route) {
	(void)uri_loc;
	(void)route;
}

void	add_string_to_envp(std::vector<const char*>&env, std::string str)
{
	env.push_back(str.c_str());
}

void	Response::process_post(const Location* uri_loc, const str_t& route) {
	std::vector<const char *>	env;
	Request::fields_t::const_iterator	it;

	env.push_back("REQUEST_METHOD=POST");

	str_t content_type = "CONTENT_TYPE=";
	it = _request->get_headers().find("content-type");
	it != _request->get_headers().end() ? content_type += it->second : "";
	env.push_back(content_type.c_str());

	str_t cookie = "HTTP_COOKIE=";
	it = _request->get_headers().find("cookie");
	it != _request->get_headers().end() ? cookie += it->second : "";
	env.push_back(cookie.c_str());

	str_t length = "CONTENT_LENGTH=";
	it = _request->get_headers().find("content-length");
	it != _request->get_headers().end() ? length += it->second : "";
	env.push_back(length.c_str());

	size_t	found = _request->get_rl()[1].find('?');
	str_t	uri;
	str_t 	query;
	str_t	query_string;
	str_t	path_info;
	str_t	document_uri;
	str_t	request_uri;
	str_t	path_translated;
	str_t	script_filename;
	str_t	script_name;

	if (found == std::string::npos) {
		uri += _request->get_rl()[1].substr(_request->get_rl()[1].find_last_of('/'));
	}
	else {
		uri += _request->get_rl()[1].substr(_request->get_rl()[1].find_last_of('/'), found);
		query += uri.substr(found);
	}
	uri = uri.substr(1);
	query_string = "QUERY_STRING=" + query;
	env.push_back(query_string.c_str());
	path_info = "PATH_INFO=" + uri;
	env.push_back(path_info.c_str());
	document_uri = "DOCUMENT_URI=" + uri;
	env.push_back(document_uri.c_str());
	request_uri = "REQUEST_URI=" + uri + "?" + query;
	env.push_back(request_uri.c_str());

	path_translated = "PATH_TRANSLATED=" + route;
	env.push_back(path_translated.c_str());
	script_name = "SCRIPT_NAME=" + route;
	env.push_back(script_name.c_str());
	script_filename = "SCRIPT_FILENAME=" + route;
	env.push_back(script_filename.c_str());
	env.push_back("PWD=/home/zedzed/webserv");
	env.push_back("PATH=/home/zedzed/.cargo/bin:/home/zedzed/.local/bin:/home/zedzed/bin:/usr/local/bin:/usr/local/sbin:/usr/bin:/usr/sbin");
	const char *argv [] = {"cgi", "/home/zedzed/webserv/www/form/submit.php", NULL};
	int	fd = open("/tmp/infile", O_WRONLY | O_CREAT, 0644);
	write(fd, _request->get_body().c_str(), _request->get_body().size());
	close(fd);
	fd = open("/tmp/infile", O_RDONLY);
	pid_t pid = fork();
	if (pid == -1)
		exit(0);
	else if (pid == 0)
	{
		if (dup2(fd, STDIN_FILENO) == -1)
			std::cout << "error\n";
		execve("/usr/bin/php-cgi", (char**)argv, (char**)(env.begin().base()));
		printf("error\n");
		exit(1);
	}
	close(fd);
	wait(0);
	if (uri_loc->get_cgi().empty())
		error_response(501);
	(void)route;
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

	pos = std::string::npos;
	path = _request->get_rl()[1];
	do {
		pos = path.find_last_of('/', pos);
		it = get_server()->get_routes().find(path.substr(0, pos + 1));
		if (it != get_server()->get_routes().end()) {
			route = it->second->get_root() + path.substr(pos + 1); // to do: test si root fini /
			return it->second;
		}
		pos ? pos -= 1 : 0;
	} while (pos);
	return NULL;
}

void	Response::process_method(const Location* uri_loc, const str_t& route) {
	str_t	method;
	bool	redir;

	method = _request->get_rl()[0];
	redir = uri_loc->get_redir().first != -1 ? true : false;
	if (_method_allowed(uri_loc, method) == false)
		error_response(405);
	else if (redir == true) {
		if (_extract_content(&uri_loc->get_redir().second) == FAILURE)
			return error_response(404);
		_set_header(301, &uri_loc->get_redir().second);
	}
	else if (method == "POST")
		process_post(uri_loc, route);
	else if (method == "GET")
		process_get(uri_loc, route);
	else if (method == "DELETE")
		process_delete(uri_loc, route);
}
