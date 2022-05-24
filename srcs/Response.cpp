#include "Response.hpp"

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

void	Response::_set_header(int code, const str_t* redir) { // search how to format time like "Date: Fri, 22 Apr 2022 07:31:04 GMT"
	str_t	data;
	str_t	time;

	data += "HTTP/1.1" + _itoa(code) + code_g[code] + CRLF;
	data += "Server: " SERVER CRLF;
	data += time + CRLF; // to do
	data += "Content-Type: text/html" CRLF; // mimes_type
	data += "Content-Length: " + _itoa(_buffer.size()) + CRLF;
	if (redir)
		data += "Location: " + (*redir) + CRLF;
	if (redir || code == 200 || code == ERR_403 || code == ERR_405)
		data += "Connection: Keep-alive" CRLF;
	else
		data += "Connection: close" CRLF;
	data += _buffer;
	_buffer = data;
}

void	Response::process_get(const Location* uri_loc, const str_t& route) {
	struct stat	st;
	bool	is_dir;

	stat(route.c_str(), &st);
	is_dir = (st.st_mode & S_IFMT) == S_IFDIR;	
	if (is_dir) {
		if (uri_loc->get_autoindex() == false)
			return error_response(ERR_403);
		//to do: readdir 
	}
	else if (_extract_content(&route) == FAILURE)
		return static_cast<void>(_buffer += page_g[ERR_500]);
	_set_header(200);
}

void	Response::process_delete(const Location* uri_loc, const str_t& route) {
	(void)uri_loc;
	(void)route;
}

void	Response::process_post(const Location* uri_loc, const str_t& route) {
	if (uri_loc->get_cgi().empty())
		return error_response(ERR_500);
	(void)route;
}

void	Response::error_response(int code, const str_t* redir) {
	Server::pages_t::const_iterator	it;

	_buffer.clear();
	if (_server) {
		it = _server->get_err_pages().find(code); //to do: change code in utils 
		if (it != _server->get_err_pages().end()) {
			if (_extract_content(&it->second) == FAILURE)
				return static_cast<void>(_buffer += page_g[ERR_500]);
		}
	}
	if (_buffer.empty()) 
		_buffer += page_g[code];
	if (_request->get_rl()[2].substr(0, 5) == "HTTP/")
		_set_header(code, redir);
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
		it = get_server()->get_routes().find(path.substr(0, pos));
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

	method = _request->get_rl()[0];
	if (_method_allowed(uri_loc, method) == false)
		error_response(ERR_405);
	else if (uri_loc->get_redir().first != -1)
		error_response(301, &(uri_loc->get_redir().second));
	else if (method == "POST")
		process_post(uri_loc, route);
	else if (method == "GET")
		process_get(uri_loc, route);
	else if (method == "DELETE")
		process_delete(uri_loc, route);
}
