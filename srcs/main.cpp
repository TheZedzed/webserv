#include "HttpContext.hpp"
#include "Parser.hpp"

static HttpContext	*webserver;
static Parser		*parser;

static void	handler(int signum) {
	ERR_LOG(signum, "Stop server with signal!");
	delete parser;
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
		ev.data.fd = it->first;
		ev.events = EPOLLIN;
		res = epoll_ctl(instance, EPOLL_CTL_ADD, it->first, &ev);
		if (res == -1)
			return FAILURE;	
	}
	return SUCCESS;
}

static bool	build_events(Event::Pool& pool) {
	Parser::Listenning::const_iterator	it;
	struct sockaddr_in	addr;
	int		socket_fd;
	int		res;

	res = 1;
	it = parser->getMap().begin();
	for (; it != parser->getMap().end(); ++it) {
		bzero(&addr, sizeof(addr));
		socket_fd = socket(AF_INET, SOCK_STREAM, 0);
		setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &res, sizeof(res));
		addr.sin_family = AF_INET; // ipv4
		addr.sin_addr.s_addr = htonl(inet_addr(it->first.first.c_str()));
		addr.sin_port =  htons(std::atoi(it->first.second.c_str()));
		if (bind(socket_fd, (sockaddr *)&addr, sizeof(sockaddr_in)))
			return FAILURE;
		if (listen(socket_fd, 100)) // up to 100 connections
			return FAILURE;
		pool.insert(std::make_pair(socket_fd, it->second));
	}
	return SUCCESS;
}

int	main(int ac, char **av) {
	Event::Pool	events;
	int			epoll;

	signal(SIGINT, handler);
	try {
		if (ac > 2)
			throw std::logic_error("Usage: ./webserve [config_file]");
		parser = new Parser(ac == 1 ? "ez.conf" : av[1]);
		if (build_events(events))
			throw std::runtime_error("Failed build events");
		if (listenning(events, epoll))
			throw std::runtime_error("Failed start listenning on servers");
		webserver = new HttpContext(events, epoll);
		webserver->loop();
	}
	catch (const std::exception& error) {
		std::cerr << error.what() << std::endl;
	}
	delete parser;
	delete webserver;
	return 0;
}