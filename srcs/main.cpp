#include "Parser.hpp"

static HttpContext	*webserver;

static void	handler(int signum) {
	ERR_LOG(signum, "Stop server with signal!");
	delete webserver;
	exit(0);
}

#if DEBUG
void	print_map(const Parser::Listenning& map) {
	Parser::Listenning::const_iterator	it;
	Event::Servers::const_iterator	it1;

	it = map.begin();
	for (; it != map.end(); ++it) {
		it1 = it->second.begin();
		std::cout << "Socket: " << it->first.first << ":" << it->first.second << "\n";
		for (; it1 != it->second.end(); ++it1) {
			std::cout << *it1;
		}
	}
}

void	print_map2(const Event::Pool& map) {
	Event::Pool::const_iterator		it;
	Event::Servers::const_iterator	it1;

	std::cout << "[GOD]\n\n";
	it = map.begin();
	for (; it != map.end(); ++it) {
		it1 = it->second.begin();
		for (; it1 != it->second.end(); ++it1)
			std::cout << "\n" << *((*it1)->getConfig());
	}
}
#endif

static bool	listenning(const Event::Pool& pool, int& instance) {
	Event::Pool::const_iterator	it;
	struct epoll_event	ev;
	int	res;

	instance = epoll_create(1);
	if (instance == -1) 
		return FAILURE;
	for (it = pool.begin(); it != pool.end(); ++it) {
		fcntl(it->first, F_SETFL, O_NONBLOCK);
		res = epoll_ctl(instance, EPOLL_CTL_ADD, it->first, &ev);
		if (res == -1)
			return FAILURE;	
	}
	return SUCCESS;
}

static int	tcpSocket(const Config::Socket& sock) {
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

static bool	build_events(Parser* parser, Event::Pool& pool, int& instance) {
	Parser::Listenning::const_iterator	it;
	std::pair<int, Event::Servers>	item;

	it = parser->getMap().begin();
	for (; it != parser->getMap().end(); ++it) {
		item = std::make_pair(tcpSocket(it->first), it->second);
		pool.insert(item);
	}
#if DEBUG
	print_map2(pool);
#endif
	if (listenning(pool, instance))
		return FAILURE;
	return SUCCESS;
}

int	main(int ac, char **av) {
	Event::Pool	events;
	Parser		*parser;
	int			epoll;

	try {
		signal(SIGINT, handler);
		if (ac > 2) 
			throw "Usage: ./webserve [config_file]"; // must inhirit std::exception
		parser = new Parser(ac == 1 ? "ez.conf" : av[1]);
		if (build_events(parser, events, epoll))
			throw "Failed start listenning on servers"; // must inhirit std::exception
		webserver = new HttpContext(parser, events, epoll);
		if (webserver->loop())
			throw "Failure during execution"; // must inhirit std::exception
		delete webserver;
	}
	catch(const char* msg) {
		std::cerr << msg << std::endl;
		delete webserver;
	}
	return 0;
}