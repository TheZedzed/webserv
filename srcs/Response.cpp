#include "Response.hpp"

static str_t	get_path(const str_t& uri)
{ return uri.substr(0, uri.find_last_of('?')); }

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

void	Response::process_post(const Location* uri_loc, const str_t& route) {
	size_t	found;
	str_t	uri;
	str_t 	query;
	str_t	query_string;
	str_t	path_info;
	str_t	document_uri;
	str_t	request_uri;
	str_t	path_translated;
	str_t	script_filename;
	str_t	script_name;
	str_t	request;
	str_t 	content_type;
	str_t	content_length;
	str_t 	cookie;
	static str_t	outfile_name = "/tmp/fileout";
	static str_t	infile_name = "/tmp/filein";
	static str_t	server_name = "SERVER_NAME= " SERVER;

	std::vector<char *>	env;
	Request::fields_t::const_iterator	it;

	if (uri_loc->get_cgi().empty())
		return error_response(501);
	found =_request->get_rl()[1].find('?');
	request ="REQUEST_METHOD=POST"; //can be dynamic with get->rl[0]
	content_type = "CONTENT_TYPE=";
	it = _request->get_headers().find("content-type");
	it != _request->get_headers().end() ? content_type += it->second : "";
	cookie = "HTTP_COOKIE=";
	it = _request->get_headers().find("cookie");
	it != _request->get_headers().end() ? cookie += it->second : "";
	content_length = "CONTENT_LENGTH=";
	it = _request->get_headers().find("content-length");
	it != _request->get_headers().end() ? content_length += it->second : content_length += "";
	if (found == std::string::npos) {
		uri += _request->get_rl()[1].substr(_request->get_rl()[1].find_last_of('/'));
	}
	else {
		uri += _request->get_rl()[1].substr(_request->get_rl()[1].find_last_of('/'), found);
		query += uri.substr(found);
	}
	uri = uri.substr(1);
	query_string = "QUERY_STRING=" + query;
	path_info = "PATH_INFO=" + uri;
	document_uri = "DOCUMENT_URI=" + uri;
	request_uri = "REQUEST_URI=" + uri + "?" + query;
	path_translated = "PATH_TRANSLATED=" + route;
	script_name = "SCRIPT_NAME=" + route;
	script_filename = "SCRIPT_FILENAME=" + route;

	env.push_back(request.begin().base());
	env.push_back(content_type.begin().base());
	env.push_back(cookie.begin().base());
	env.push_back(content_length.begin().base());
	env.push_back(query_string.begin().base());
	env.push_back(path_info.begin().base());
	env.push_back(document_uri.begin().base());
	env.push_back(request_uri.begin().base());
	env.push_back(path_translated.begin().base());
	env.push_back(script_name.begin().base());
	env.push_back(script_filename.begin().base());
	env.push_back(server_name.begin().base());
	env.push_back(NULL);

	int in;
	int out;
	out = open(outfile_name.c_str(), O_RDWR | O_CREAT, 0644);
	in = open(infile_name.c_str(), O_WRONLY | O_CREAT, 0644);
	write(in, _request->get_body().c_str(), _request->get_body().size());
	close(in);
	in = open(infile_name.c_str(), O_RDONLY);

	pid_t pid = fork();
	if (pid == -1)
		exit(0);
	else if (pid == 0)
	{
		if (dup2(in, STDIN_FILENO) == -1)
			exit(FAILURE);
		if (dup2(out, STDOUT_FILENO) == -1)
			exit(FAILURE);
		for (int i = 3; i < 256; ++i)
			close(i);
		char * const argv[] = {(char*)"cgi", const_cast<char *>(route.c_str()), NULL};
		execve(uri_loc->get_cgi().c_str(), argv, env.begin().base());
		perror(0);
		exit(1);
	}
	int status;
	waitpid(pid, &status, 0);
	close(in);
	close(out);
	unlink(infile_name.c_str());
	if (WEXITSTATUS(status))
	{
		unlink(outfile_name.c_str());
		return error_response(500);
	}

	//if(_extract_content(&outfile_name) == FAILURE)
		//return error_response(500);
	str_t 	line;
	str_t 	headers;

	std::ifstream output(outfile_name.c_str());        //SPLIT THE FD INTO HEADERS PART AND BODY PART
	//HEADERS
	while(std::getline(output, line) && line != "\r")
	{
		line += "\n";
		headers.append(line);
	}
	//BODY
	size_t pos;
	size_t length;

	std::getline(output, line);
	pos = output.tellg();

	output.seekg(pos, output.end);
	length = (size_t)output.tellg() - headers.size();
	output.seekg(pos, output.beg);
	char buffer_body[length + 1];
	buffer_body[length + 1] = '\0';
	output.readsome(buffer_body, length);

	unlink(outfile_name.c_str());
		str_t	data;
	str_t	time;

	_buffer = data;

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
			route = it->second->get_root() + path.substr(pos + 1);
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
