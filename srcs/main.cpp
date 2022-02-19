#include "HttpContext.hpp"
#include <fstream>
#include <sstream>

static const char	*g_ser[] = {"400", "403", "404", "405", "408", "414", NULL};
static const char	*g_cli[] = {"500", "505", NULL};
static const char	*g_ret[] = {"300", "301", "302", "306", NULL};

static HttpContext	*save;

static void	handler(int signum) {
	ERR_LOG(signum, "Stop server with signal!");
	delete save;
	exit(0);
}

int	tcpSocket(const Config::Socket& sock) {
	struct sockaddr_in	listen_address;
	int		socket_fd;
	int		res;

	res = 1;
	socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &res, sizeof(res));
	memset(&listen_address, 0, sizeof(sockaddr_in));
	listen_address.sin_family = AF_INET; //ipv4
	listen_address.sin_port = htons(std::atoi(sock.second.c_str()));
	listen_address.sin_addr.s_addr = htonl(inet_addr(sock.first.c_str()));
	res = bind(socket_fd, reinterpret_cast<sockaddr *>(&listen_address), sizeof(sockaddr_in));
	res = listen(socket_fd, 100);//up to 100 connections
	return socket_fd;
}

Array&	split(const String& buffer, Array& line) {
	std::stringstream	tmp(buffer);
	String	elem;

	while (tmp >> elem) {
		line.push_back(elem);
	}
	for (int i = 0; i < (int)line.size(); ++i)
		printf("[%s] ", line[i].c_str());
	printf("\n");
	return line;
}

bool	names_directive(Config*& config, const Array& line, int flag) {
	if (line.size() < 3 || *line.rbegin() != ";")
		return FAILURE;
	if (flag)
		config->setNames(line);
	return SUCCESS;
}

bool	clienSize_directive(Config*& config, const Array& line, int flag) {
	char	*end;

	if (line.size() != 3 || *line.rbegin() != ";")
		return FAILURE;
	std::strtol(line[1].c_str(), &end, 10);
	if (*end || line[1].size() > 4)
		return FAILURE;
	if (flag)
		config->setMax(line[1]);
	return SUCCESS;
}

bool	wrong_serverDirective(Config*& config, const Array& line, int flag) {
	if (line[0] != "server")
		return FAILURE;
	if (line.size() != 2 || *line.rbegin() != "{")
		return FAILURE;
	if (flag)
		config = new Config();
	return SUCCESS;
}

/* tuple host:port mandatory */
bool	listen_directive(Config*& config, const Array& line, int flag) {
	static Config::Sockets			tmp;
	Config::Sockets::const_iterator	it;
	String	port;
	String	host;
	size_t	pos;
	char*	end;

	it = tmp.begin();
	if (line.size() != 3 || *line.rbegin() != ";")
		return FAILURE;
	pos = line[1].find(':');
	host = line[1].substr(0, pos);
	port = line[1].substr(pos + 1);
	std::strtol(port.c_str(), &end, 10);
	if (port.size() > 5 || *end)
		return FAILURE;
	for (; it != tmp.end(); ++it) { // checks same tuple host:port
		if (it->first == host && it->second == port)
			return FAILURE;
	}
	tmp.push_back(std::make_pair(host, port));
	if (flag)
		config->setSockets(tmp);
	return SUCCESS;
}

bool	errPage_directive(Config*& config, const Array& line, int flag) {
	int	i = 0;

	if (line.size() != 4 || *line.rbegin() != ";")
		return FAILURE;
	while (g_cli[i] && line[1] != String(g_cli[i]))
		++i;
	if (!g_cli[i]) {
		i = 0;
		while (g_ser[i]&& line[1] != String(g_ser[i]))
			++i;
		if (!g_ser[i])
			return FAILURE;
	}
	if (flag)
		config->setErrorPage(line);
	return SUCCESS;
}

bool	allow_directive(const Array& line, Location*& loc, int flag) {
	if (line.size() == 1 || *line.rbegin() != ";")
		return FAILURE;
	if (flag)
		loc->setMethod(line);
	return SUCCESS;
}

bool	cgi_directive(const Array& line, Location*& loc, int flag) {
	if (line.size() != 3 || *line.rbegin() != ";")
		return FAILURE;
	if (flag)
		loc->setCgi(line[1]);
	return SUCCESS;
}

bool	return_directive(const Array& line, Location*& loc, int flag) {
	int	i = 0;

	if (line.size() != 4 || *line.rbegin() != ";")
		return FAILURE;
	while (line[1] != String(g_ret[i]))
		++i;
	if (!g_ret[i])
		return FAILURE;
	if (flag)
		loc->setRedirection(line);
	return SUCCESS;
}

bool	autoIndex_directive(const Array& line, Location*& loc, int flag) {
	if (line.size() != 3 || *line.rbegin() != ";")
		return FAILURE;
	else if (line[1] != "on" && line[1] != "off")
		return FAILURE;
	if (flag)
		loc->setAutoIndex(line[1]);
	return SUCCESS;
}

bool	root_directive(const Array& line, Location*& loc, int flag) {
	if (line.size() != 3 || *line.rbegin() != ";")
		return FAILURE;
	if (flag)
		loc->setRoot(line[1]);
	return SUCCESS;
}

bool	wrong_ldirective(const Array& line, Location*& loc, int flag) {
	bool	(*ptr)(const Array&, Location*&, int);

	ptr = NULL;
	if (line[0] == "allow")
		ptr = &allow_directive;
	else if (line[0] == "cgi")
		ptr = &cgi_directive;
	else if (line[0] == "return")
		ptr = &return_directive;
	else if (line[0] == "autoindex")
		ptr = &autoIndex_directive;
	else if (line[0] == "root")
		ptr = &root_directive;
	if (ptr)
		return (*ptr)(line, loc, flag);
	return FAILURE;
}

bool	location_directive(std::ifstream& in, Array& line, Config*& config, int flag) {
	Location*	curr_loc;
	String		buffer;
	String		key;

	if (line.size() != 3 || *line.rbegin() != "{")
		return FAILURE;
	if (flag) {
		key = line[1];
		curr_loc = new Location();
	}
	while (std::getline(in, buffer)) {
		line.clear();
		line = split(buffer, line);
		if (line.size() == 0)
			continue ;
		if (line[0] == "}" && line.size() == 1)
			break ;
		else if (line[0] == "}" && line.size() != 1)
			return FAILURE;
		else if (wrong_ldirective(line, curr_loc, flag))
			return FAILURE;
	}
	if (flag)
		config->setRoutes(key, &curr_loc);
	return SUCCESS;
}

bool	wrong_sdirective(std::ifstream& in, Array& line, Config*& config, int flag) {
	bool	(*ptr)(Config*&, const Array&, int);

	ptr = NULL;
	if (line[0] == "listen")
		ptr = &listen_directive;
	else if (line[0] == "error_page")
		ptr = &errPage_directive;
	else if (line[0] == "max_client_body_size")
		ptr = &clienSize_directive;
	else if (line[0] == "server_name")
		ptr = &names_directive;
	else if (line[0] == "location")
		return location_directive(in, line, config, flag);
	if (ptr)
		return (*ptr)(config, line, flag);
	return FAILURE;
}

bool	parse(Event::Pool* events, const String& file, int flag) {
	typedef std::map<Config::Socket, Event::Servers>	Dad;
	Dad	daddy;
	std::ifstream	in;
	Server*		curr_serv;
	Config*		curr_conf;
	String		buffer;
	Array		line;
	int		error = 0;

	in.open(file.c_str());
	if (!in.is_open())
		error = 1;
	while (!error && std::getline(in, buffer)) {
		line.clear();
		line = split(buffer, line);
		if (line.size() == 0)
			continue ;
		if (line[0] == "}" && line.size() == 1)
			break ;
		else if (line[0] == "}" && line.size() != 1)
			error = 3;
		else if (!wrong_serverDirective(curr_conf, line, flag)) {
			while (!error && std::getline(in, buffer)) {
				line.clear();
				line = split(buffer, line);
				if (line.size() == 0)
					continue ;
				if (line[0] == "}" && line.size() == 1)
					break ;
				else if (line[0] == "}" && line.size() != 1)
					error = 3;
				else if (wrong_sdirective(in, line, curr_conf, flag))
					error = 4;
			}
			if (flag) {
				Config::Sockets::const_iterator	it;
				Event::Servers	servers;
				Dad::iterator	res;

				curr_serv = new Server(curr_conf);
				for (it = curr_conf->getSockets().begin(); it != curr_conf->getSockets().end(); ++it) {
					res = daddy.find(*it);
					if (res != daddy.end())
						res->second.push_back(curr_serv);
					else {
						servers.clear();
						servers.push_back(curr_serv);
						daddy.insert(std::make_pair(*it, servers));
					}
				}
			}
		}
		else
			error = 2;
	}
	if (flag) {
		Dad::iterator	it1;
		int	socket;

		for (it1 = daddy.begin(); it1 != daddy.end(); ++it1) {
			socket = tcpSocket(it1->first);
			events->insert(std::make_pair(socket, it1->second));
		}
	}
	printf("error: %d\n", error);
	in.close();
	return error & 1;
}

bool	listenning(const Event::Pool& pool, int* instance) {
	Event::Pool::const_iterator	it;
	struct epoll_event	ev;
	int	res;

	*instance = epoll_create(1);
	if (*instance == -1) 
		return FAILURE;
	for (it = pool.begin(); it != pool.end(); ++it) {
		fcntl(it->first, F_SETFL, O_NONBLOCK);
		res = epoll_ctl(*instance, EPOLL_CTL_ADD, it->first, &ev);
		if (res == -1)
			return FAILURE;	
	}
	return SUCCESS;
}

int	main(int ac, char **av) {
	//HttpContext	*webserver = NULL;
	Event::Pool	events;
	//int		epoll;

	try {
		signal(SIGINT, handler);
		if (ac > 2) 
			throw "Usage: ./webserve [config_file]"; // must inhirit std::exception
		if (parse(&events, ac == 1 ? "ez.conf" : String(av[1]), 0))
			throw "Wrong config file"; // must inhirit std::exception
		/*
		parse(&events, ac == 1 ? "ez.couf" : String(av[1]), 1);
		if (listenning(events, &epoll))
			throw "Failed start listenning on servers"; // must inhirit std::exception
		webserver = new HttpContext(events, epoll);
		save = webserver;
		if (webserver->loop())
			throw "Failure during execution"; // must inhirit std::exception
		delete webserver;
		*/
	}
	catch(const char* msg) {
		std::cerr << msg << std::endl;
		//delete webserver;
	}
	return 0;
}