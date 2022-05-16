#include "HttpContext.hpp"

static HttpContext	*webserver;

static void	handler(int signum) {
	if (signum == SIGQUIT) {
		delete webserver;
		exit(128 + SIGQUIT);
	}
}

int	main(int ac, char **av) {
	signal(SIGQUIT, handler);
	signal(SIGINT, SIG_IGN);
	signal(SIGTSTP, SIG_IGN);
	try {
		if (ac > 2)
			throw std::logic_error("Usage: ./webserve [config_file]");
		webserver = new HttpContext(ac == 1 ? "ez.conf" : av[1]);
		webserver->worker();
	}
	catch (const std::exception& error) {
		std::cerr << error.what() << std::endl;
		delete webserver;
	}
	return 0;
}
