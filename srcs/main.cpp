#include "HttpContext.hpp"
#include "Parser.hpp"

static HttpContext	*webserver;
static Parser		*parser;

static void	handler(int signum) {
	delete parser;
	delete webserver;
	exit(0);
}

static bool	listenning(const HttpContext::events_t& pool, int& instance) {
	HttpContext::events_t::const_iterator	it;
	struct epoll_event	ev;
	int	res;

	instance = epoll_create(1);
	if (instance == -1) 
		return FAILURE;
	for (it = pool.begin(); it != pool.end(); ++it) {
		ev.data.ptr = it->second;
		ev.events = EPOLLIN;
		res = epoll_ctl(instance, EPOLL_CTL_ADD, it->first, &ev);
		if (res == -1)
			return FAILURE;	
	}
	return SUCCESS;
}

static bool	build_events(HttpContext::events_t& pool) {
	Parser::listenners_t::const_iterator	it;
	Connection*	listenner;
	sockaddr_in	addr;
	int		socket_fd;
	int		opt;

	opt = 1;
	it = parser->getMap().begin();
	for (; it != parser->getMap().end(); ++it) {
		bzero(&addr, sizeof(addr));
		socket_fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
		setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
		addr.sin_family = AF_INET; // ipv4
		addr.sin_addr.s_addr = htonl(inet_addr(it->first.first.c_str()));
		addr.sin_port =  htons(std::atoi(it->first.second.c_str()));
		if (bind(socket_fd, (sockaddr *)&addr, sizeof(sockaddr_in)))
			return FAILURE;
		if (listen(socket_fd, 100)) // up to 100 connections
			return FAILURE;
		listenner = new Connection(socket_fd, LISTENNER);
		listenner->setData(it->second);
		pool.insert(std::make_pair(socket_fd, listenner));
	}
	return SUCCESS;
}

int	main(int ac, char **av) {
	HttpContext::events_t	events;
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