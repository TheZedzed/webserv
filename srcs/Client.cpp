#include "Client.hpp"

static int	delete_file(const char* path, const struct stat* sb, int flag, struct FTW* buf) {
	(void)flag;
	(void)sb;
	(void)buf;
	return remove(path);
}

static bool	_method_allowed(const Location* uri_loc, const str_t& method) {
	strs_t::const_iterator	it;

	it = uri_loc->get_allowed().begin();
	for (; it != uri_loc->get_allowed().end(); ++it) {
		if (*it == method)
			return true;
	}
	return false;
}

static bool	_route_error(str_t& route, int& state) {
	struct stat	st;

	if (stat(route.c_str(), &st) == -1) {
		if (errno == EACCES)
			state |= (ERROR | ERR_403);
		else if (errno == ENOENT)
			state |= (ERROR | ERR_404);
		else if (errno == ENAMETOOLONG)
			state |= (ERROR | ERR_414);
		else
			state |= (ERROR | ERR_500);
		return true;
	}
	if ((st.st_mode & S_IFMT) == S_IFDIR && *route.rbegin() != '/')
		route += "/";
	return false;
}

Client::Client(const servers_t& serv) : _request(new Request()), _response(NULL), _servers(serv)
{ std::cout << "Create new client!" << std::endl; }

Client::~Client() {
	std::cout << "Destroy client!" << std::endl;
	delete _request;
	delete _response;
}

void	Client::clear() {
	delete _response;
	delete _request;
	raw_data.clear();
	_request = new Request();
	_response = NULL;
}

Request*	Client::get_request()
{ return _request; }

Response*	Client::get_response()
{ return _response; }

void	Client::set_response(Response* response)
{ _response = response; }

void	Client::set_request(Request* request)
{ _request = request; }

const Server*	Client::requested_server() const {
	servers_t::const_iterator			it1;
	strs_t::const_iterator				it2;
	str_t								match;
	Request::fields_t::const_iterator	res;

	it1 = _servers.begin();
	res = _request->get_headers().find("host");
	match = res->second.substr(0, res->second.find_last_of(':')); // host: [name:port]
	for (; it1 != _servers.end(); ++it1) {
		it2 = (*it1)->get_names().begin();
		for (; it2 != (*it1)->get_names().end(); ++it2) {
			if (*it2 == match)
				return *it1;
		}
	}
	return _servers[0];
}

void	Client::_process_get(const Location* uri_loc, const str_t& route) {
	bool	autoindex;
	str_t	subroute;

	autoindex = uri_loc->get_autoindex();
	subroute = route.substr(uri_loc->get_root().size() - 1);
	if (*route.rbegin() == '/') {
		if (autoindex == false)
			_response->error_response(403);
		else if (_response->extract_directory(route, subroute) == FAILURE)
			_response->error_response(500);
		else
			_response->set_header(200);
	}
	else if (_response->extract_content(&route) == FAILURE)
		_response->error_response(500);
	else
		_response->set_header(200);
	return ;
}

void	Client::_process_delete(const str_t& path) {
	// to do: delete in DELETE folder only else error 501
	if (*path.rbegin() == '/') {
		if (nftw(path.c_str(), delete_file, 20, FTW_DEPTH | FTW_PHYS) < 0)
			_response->error_response(500);
		else
			_response->set_header(200);
	}
	else if (remove(path.c_str()) != SUCCESS)
		_response->error_response(500);
	else
		_response->set_header(200);
	return ;
}

void	Client::_process_redir(const Location* uri_loc) {
	str_t	redirection;

	redirection = uri_loc->get_redir().second;
	if (_response->extract_content(&redirection) == FAILURE)
		_response->error_response(404);
	else
		_response->set_header(301, redirection);
	return ;
}

void	Client::_process_post(const Location* uri_loc, const str_t& route) {
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
		return _response->error_response(501);
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
		return _response->error_response(500);
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

	//_buffer = data;

}

void	Client::process_method(const Location* uri_loc, const str_t& route) {
	str_t	method;
	bool	redir;

	method = _request->get_rl()[0];
	redir = uri_loc->get_redir().first != -1;
	if (_method_allowed(uri_loc, method) == false)
		_response->error_response(405);
	else if (redir == true)
		_process_redir(uri_loc);
	else if (method == "POST")
		_process_post(uri_loc, route);
	else if (method == "GET")
		_process_get(uri_loc, route);
	else if (method == "DELETE")
		_process_delete(route);
}

/*
** to do: describe function
*/
void	Client::process_response(int& state) {
	const Location*	uri_loc = NULL;
	str_t	route;

	if (state & ERROR)
		_response->error_response(state & ~(RESPONSE | ERROR));
	else if (_request->get_body().size() > requested_server()->get_max())
		_response->error_response(413);
	else {
		_response->construct_route(&uri_loc, route);
		if (_route_error(route, state) == true)
			_response->error_response(state & ~(RESPONSE | ERROR));
		else
			process_method(uri_loc, route);
	}
	return ;
}
