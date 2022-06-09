#include "HttpContext.hpp"

static HttpContext	*webserver;

static void	handler(int signum, siginfo_t *info, void* context) {
	(void)context;

	if (signum == SIGINT || signum == SIGTSTP)
		signal(signum, SIG_IGN);
	else if (signum == SIGALRM)
		webserver->timeout(info->si_value.sival_ptr);
	else {
		delete webserver;
		exit(128 + SIGQUIT);
	}
}

static void	init(int ac) {
	struct sigaction	sigg;

	bzero(&sigg, sizeof(struct sigaction));
	sigg.sa_flags = SA_SIGINFO;
	sigg.sa_sigaction = &handler;
	if (ac > 2)
		throw std::logic_error("Usage: ./webserv [config_file]");
	if (sigaction(SIGQUIT, &sigg, NULL) == -1)
		throw std::logic_error("Error sigaction!");
	if (sigaction(SIGINT, &sigg, NULL) == -1)
		throw std::logic_error("Error sigaction!");
	if (sigaction(SIGTSTP, &sigg, NULL) == -1)
		throw std::logic_error("Error sigaction!");
	if (sigaction(SIGALRM, &sigg, NULL) == -1)
		throw std::logic_error("Error sigaction!");
}

int	main(int ac, char **av) {
	try {
		init(ac);
		webserver = new HttpContext(ac == 1 ? "ez.conf" : av[1]);
		webserver->worker();
	}
	catch (const std::exception& error) {
		std::cerr << error.what() << std::endl;
		delete webserver;
	}
	return 0;
}
