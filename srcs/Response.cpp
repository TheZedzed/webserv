#include "Response.hpp"
#include <sys/stat.h>

static bool	_is_dir(struct stat *st, const str_t& route) {
	bzero(&st, sizeof(struct stat));
	if (stat(route.c_str(), st) == -1)
		return FAILURE;
	return SUCCESS;
}

Response::Response(const Server* serv, const Request* req) : _server(serv), _request(req)
{ std::cout << "Create Response" << std::endl; }

Response::~Response()
{ std::cout << "Destroy Response" << std::endl; }

const Server*	Response::get_server() const
{ return _server; }

str_t	Response::_extract_page(int& code, const str_t* path) const {
	std::ifstream	infile;
	str_t	buffer;
	str_t	line;

	_init_error_pages();
	if (path == NULL) // default page
		return page_g[code];
	infile.open(path->c_str());
	if (!infile.is_open() && (code = 500)) // error openning file
		return page_g[500];
	else { // append infile content
		while (std::getline(infile, line))
			buffer.append(line);
		infile.close();
		return buffer;
	}
}

void	Response::_set_header(str_t& buffer, const str_t& redir) const { // search how to format time like "Date: Fri, 22 Apr 2022 07:31:04 GMT"
	str_t	data;
	str_t	time;

	data += "HTTP/1.1 302" + code_g[302] + CRLF;
	data += "Server: " SERVER CRLF;
	data += time + CRLF; // to do
	data += "Content-Type: text/html" CRLF; // mimes_type
	data += "Content-Length: " + _itoa(buffer.size()) + CRLF;
	data += "Location: " + redir + CRLF; // reformat redir
	data += "Connection: Keep-alive" CRLF;
	data += buffer;
	buffer = data;
}

void	Response::_set_header(str_t& buffer, int code) const { // search how to format time like "Date: Fri, 22 Apr 2022 07:31:04 GMT"
	str_t	data;
	str_t	time;

	data += "HTTP/1.1" + _itoa(code) + code_g[code] + CRLF;
	data += "Server: " SERVER CRLF;
	data += time + CRLF; // to do
	data += "Content-Type: text/html" CRLF; // mimes_type
	data += "Content-Length: " + _itoa(buffer.size()) + CRLF;
	if (code == 200 || code == 403 || code == 405)
		data += "Connection: Keep-alive" CRLF;
	else
		data += "Connection: close" CRLF;
	data += buffer;
	buffer = data;
}

str_t	Response::code_response(const Location* uri_loc, int code) const {
	Server::pages_t::const_iterator	it;
	str_t	protocol;
	str_t	buffer;
	str_t	redir;

	protocol = _request->get_rl()[2].substr(0, 4);
	if (code != 200) {
		redir = uri_loc->get_redir().second; // todo: format
		it = _server->get_err_pages().find(code);
		if (code == 302)
			buffer += _extract_page(code, &redir);
		else if (it != _server->get_err_pages().end())
			buffer += _extract_page(code, &it->second);
		else
			buffer += _extract_page(code, NULL);
	}
	if (protocol == "HTTP/")
		code != 302 ? _set_header(buffer, code) : _set_header(buffer, redir);
	return buffer;
}

/*
** Aim: search and expand route requested
** return server requested location block
*/
const Location*	Response::construct_route(str_t& route) const {
	Location*	loc(NULL);
	str_t	tmp;

	// replace with location in _server
	// stat tmp
	route = tmp;
	return loc;
}

str_t	Response::_method_in_route(const Location* uri_loc) const {
	strs_t::const_iterator	it;
	str_t	method;

	method = _request->get_rl()[0];
	it = uri_loc->get_allowed().begin();
	for (; it != uri_loc->get_allowed().end(); ++it) {
		if (*it == method) {
			method = *it;
			break ;
		}
	}
	return method;
}

str_t	Response::_process_get(const Location* uri_loc, bool is_dir) const {
	if (is_dir && uri_loc->get_autoindex() == false)
		return code_response(uri_loc, 403);
	return str_t("get");
}

str_t	Response::_process_delete(const Location* uri_loc) const {
	(void)uri_loc;
	return str_t("delete");
}

str_t	Response::_process_post(const Location* uri_loc) const {
	if (uri_loc->get_cgi().empty())
		return code_response(uri_loc, 501);
	return str_t("post");
}

str_t	Response::process_method(const Location* uri_loc, const str_t& route) const {
	struct stat	st;
	str_t	method;
	str_t	buffer;

	method = _method_in_route(uri_loc);
	if (uri_loc->get_redir().first != -1)
		buffer += code_response(uri_loc, 312);
	else if (method.empty())
		buffer += code_response(uri_loc, 405);
	else if (_is_dir(&st, route) == FAILURE) // failure stat route
		buffer += code_response(uri_loc, 500);
	else if (method == "POST")
		buffer += _process_post(uri_loc);
	else if (method == "GET")
		buffer += _process_get(uri_loc, ((st.st_mode & S_IFMT) == S_IFDIR));
	else if (method == "DELETE")
		buffer += _process_delete(uri_loc);
	return buffer;
}
