#include "Cgi.hpp"

str_t	Cgi::_outfile_name = "/tmp/fileout";
str_t	Cgi::_infile_name = "/tmp/filein";
str_t	Cgi::_request_scheme = "REQUEST_SCHEME=http";
str_t	Cgi::_server_protocol = "SERVER_PROTOCOL=HTTP/1.1";
str_t	Cgi::_gateway_interface = "GATEWAY_INTERFACE=CGI/1.1";
str_t	Cgi::_server_software = "SERVER_SOFTWARE=webserv";


void	Cgi::_find_and_fill_req_field(const str_t& to_find, str_t& to_fill) {
	Request::fields_t::const_iterator	it;

	it = _request.get_headers().find(to_find);
	if (it != _request.get_headers().end())
		to_fill += it->second;
}

void	Cgi::_insert_env_vars(void) {

	str_t *pointer = &_server_name;

	for (int i = 0; i < 15; ++i)
		_env.push_back((pointer + i)->begin().base());
	_env.push_back(Cgi::_request_scheme.begin().base());
	_env.push_back(Cgi::_gateway_interface.begin().base());
	_env.push_back(Cgi::_server_protocol.begin().base());
	_env.push_back(Cgi::_server_software.begin().base());
	_env.push_back(Cgi::_request_scheme.begin().base());
	_env.push_back(NULL);
}

void	Cgi::_fill_remote_vars(int socketfd) {
	struct sockaddr_in	addr;
	socklen_t			socklen;
	char				buf_addr[sizeof(addr)];

	bzero(&addr, sizeof(addr));
	socklen = sizeof(addr);
	getsockname(socketfd, (struct sockaddr*)&addr, &socklen);
	_remote_port += _itoa(htons(addr.sin_port));
	_remote_address += inet_ntop(AF_INET, (void*)&addr.sin_addr, buf_addr, socklen);
}

void	Cgi::build_env(const str_t& route, int socket) {
	size_t	found;
	str_t	uri;
 	str_t	query;

	_request_method += _request.get_rl()[0];
	_find_and_fill_req_field("content-type", _content_type);
	_find_and_fill_req_field("cookie", _cookie);
	_find_and_fill_req_field("content-length", _content_length);
	_find_and_fill_req_field("host", _server_name);
	if ((found = _server_name.find(':')) != std::string::npos) {
		_server_port += _server_name.substr(found + 1);
		_server_name.erase(found);
	}
	else
		_server_port += "80";
	found = _request.get_rl()[1].find('?');
	if (found == std::string::npos) {
    	uri = _request.get_rl()[1].substr(_request.get_rl()[1].find_last_of('/'));
	}
 	else {
 		uri = _request.get_rl()[1].substr(_request.get_rl()[1].find_last_of('/'), found);
 		query += uri.substr(found);
 	}
 	uri = uri.substr(1);
 	_query_string += query;
 	_path_info += uri;
 	_document_uri += uri;
 	_request_uri += uri + "?" + query;
 	_path_translated += route;
 	_script_name += route;
 	_script_filename += route;
	_fill_remote_vars(socket);
	_insert_env_vars();
}

Cgi::Cgi(const Request& req, const str_t& path) : _request(req), _cgi_path(path), _in(-1), _out(-1) {

	_request_method = "REQUEST_METHOD=";
	_content_type = "CONTENT_TYPE=";
	_cookie = "HTTP_COOKIE=";
	_content_length = "CONTENT_LENGTH=";
	_server_name = "SERVER_NAME=";
 	_query_string = "QUERY_STRING=";
	_server_port = "SERVER_PORT=";
	_document_uri = "DOCUMENT_URI=";
	_request_uri = "REQUEST_URI=";
	_path_info = "PATH_INFO=";
	_path_translated = "PATH_TRANSLATED=";
	_script_name = "SCRIPT_NAME=";
	_script_filename = "SCRIPT_FILENAME=";
	_remote_port = "REMOTE_PORT=";
	_remote_address = "REMOTE_ADDRESS=";
}

Cgi::~Cgi(void) {
	close(_in);
	close(_out);
	unlink(Cgi::_infile_name.c_str());
	unlink(Cgi::_outfile_name.c_str());
}

bool    Cgi::exec_cgi(const str_t& route) {
	pid_t	pid;
	int		status;

	_out = open(Cgi::_outfile_name.c_str(), O_RDWR | O_CREAT, 0644);
	std::ofstream stream(Cgi::_infile_name.c_str());
	if (_out == -1 || !stream.is_open())
			return FAILURE;
    stream.write(_request.get_body().c_str(), _request.get_body().size());
	stream.close();

	_in = open(Cgi::_infile_name.c_str(), O_RDONLY);
	pid = fork();
	if (pid == -1)
		exit(FAILURE);
	else if (pid == 0)
 	{
 		if (dup2(_in, STDIN_FILENO) == -1)
 			exit(FAILURE);
 		if (dup2(_out, STDOUT_FILENO) == -1)
 			exit(FAILURE);
 		for (int i = 3; i < 256; ++i)
 			close(i);
 		char* const	argv[3] = {(char*)"cgi", const_cast<char *>(route.c_str()), NULL};
			execve(_cgi_path.c_str(), argv, _env.begin().base());
 		perror("CGI_ERROR");
 		exit(FAILURE);
 	}
 	status = 0;
 	waitpid(pid, &status, 0);
 	close(_in);
 	close(_out);
    if (WEXITSTATUS(status))
 		return FAILURE;
	return SUCCESS;
}

bool	Cgi::treat_cgi_output(str_t &buffer) {
	std::ifstream output;
	str_t	line;
	str_t	status;

	output.open(Cgi::_outfile_name.c_str());
	if (output.fail())
		return FAILURE;
	//HEADERS
	status = "200 OK"  CRLF;
	while(std::getline(output, line) && line != "\r")
	{
		line.push_back('\n');
		if (line.find("Status: ") == 0)
			status = line.substr(8);
		else
			buffer += line;
	}
	//BODY
	size_t pos;
	size_t length;

	pos = output.tellg();
	output.seekg(0, output.end);
	length = (size_t)output.tellg() - (size_t)pos;
	output.seekg(pos, output.beg);
	char buffer_body[length + 1];
	buffer_body[length + 1] = '\0';
	output.readsome(buffer_body, length);

	str_t date;
	str_t time;

	std::time_t fetch_time = std::time(NULL);
	time = std::asctime(std::localtime(&fetch_time)); //to do: change format
	*time.rbegin() = CR;
	time.push_back('\n');
	buffer.insert(0, "HTTP/1.1 " + status);
	if (buffer.find("Server: ") == std::string::npos)
		buffer += "Server: " SERVER CRLF;
	if (buffer.find("Date: ") == std::string::npos)
		buffer += "Date: " + time;

	if (buffer.find("Connection: ") == std::string::npos)
		buffer += "Connection: Keep-Alive" CRLF;
	if (buffer.find("Content-Length: ") == std::string::npos)
		buffer += "Content-Length: " + _itoa(length) + CRLF;
	buffer += CRLF;
	buffer.append(buffer_body, length);

	output.close();
	return SUCCESS;
}
