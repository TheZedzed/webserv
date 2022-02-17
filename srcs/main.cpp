#include "HttpContext.hpp"

static HttpContext	*save;

static void	handler(int signum) {
	ERR_LOG(signum, "Stop server with signal!");
	delete save;
	exit(0);
}

int	tcpSocket(const Socket& sock) {
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

bool	incorrect(const Array& line)
{ return true; }

bool	parse(Event::Pool* events, const String& file, int flag) {
	Server*		curr_serv;
	Config*		curr_conf;
	Location*	curr_loc;
	Array		tmp;

	//open file
	//if error
	//	return FAILURE
	while (1) { //not EOF
		//getline
		//clear tmp
		//split whitespace in tmp
		if (flag)
			curr_conf = new Config();
		if (tmp[0] == "server") {
			if (incorrect(tmp))
				return FAILURE;
			if (flag)
				curr_conf = new Config();
			while (1) {
				//getline
				//clear tmp
				//split whitespace in tmp
				if (tmp[0] == "}") {
					if (incorrect(tmp))
						return FAILURE;
					break;
				}
				else if (tmp[0] == "listen") {
					if (incorrect(tmp)) // check syntax line and if no duplicate
						return FAILURE;
					if (flag)
						curr_conf->setSocket(tmp);
				}
				else if (tmp[0] == "location") {
					String	key;

					if (incorrect(tmp))
						return FAILURE;
					if (flag) {
						key = tmp[1];
						curr_loc = new Location();
					}
					while (1) {
						//getline
						//clear tmp
						//split whitespace in tmp
						if (tmp[0] == "}") {
							if (incorrect(tmp))
								return FAILURE;
							break;
						}
						else if (tmp[0] == "allow") {
							if (incorrect(tmp))
								return FAILURE;
							if (flag)
								curr_loc->setMethod(tmp);
						}
						else if (tmp[0] == "cgi") {
							if (incorrect(tmp))
								return FAILURE;
							if (flag)
								curr_loc->setCgi(tmp[1]);
						}
						else if (tmp[0] == "return") {
							if (incorrect(tmp))
								return FAILURE;
							if (flag)
								curr_loc->setRedirection(tmp);
						}
						else if (tmp[0] == "autoindex") {
							if (incorrect(tmp))
								return FAILURE;
							if (flag)
								curr_loc->setAutoIndex(tmp[1]);
						}
						else if (tmp[0] == "root") {
							if (incorrect(tmp))
								return FAILURE;
							if (flag)
								curr_loc->setRoot(tmp[1]);
						}
						else
							return FAILURE;
					}
					if (flag)
						curr_conf->setRoutes(key, &curr_loc);
				}
				else if (tmp[0] == "error_page") {
					if (incorrect(tmp))
						return FAILURE;
					if (flag)
						curr_conf->setErrorPage(tmp);
				}
				else if (tmp[0] == "server_name") {
					if (incorrect(tmp))
						return FAILURE;
					if (flag)
						curr_conf->setNames(tmp);
				}
				else if (tmp[0] == "max_client_body_size") {
					if (incorrect(tmp))
						return FAILURE;
					if (flag)
						curr_conf->setMax(tmp[1]);
				}
				else
					return FAILURE;
			}
			if (flag) {
				std::pair<int, Server*>		item;
				int	socket;

				socket = tcpSocket(curr_conf->getSocket());
				curr_serv = new Server(curr_conf);
				item = std::make_pair(socket, curr_serv); 
				events->insert(item);
			}
		}
		else
			return FAILURE;
	}
	return SUCCESS;
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
	HttpContext	*webserver;
	Event::Pool	events;
	int		epoll;

	try {
		signal(SIGINT, handler);
		if (ac > 2) 
			throw "Usage: ./webserve [config_file]";
		if (parse(&events, ac == 1 ? "" : String(av[1]), 0))
			throw "Wrong config file";
		parse(&events, ac == 1 ? "" : String(av[1]), 1);
		if (listenning(events, &epoll))
			throw "Failed start listenning on servers";
		webserver = new HttpContext(events, epoll);
		save = webserver;
		if (webserver->loop())
			throw "Failure during execution";// must developpe FAILURE
		delete webserver;
	}
	catch(const char* msg) {
		std::cerr << msg << std::endl;
		delete webserver;
	}
	return 0;
}