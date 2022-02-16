#include "HttpContext.hpp"

static HttpContext	*save;

static void	handler(int signum) {
	ERR_LOG(signum, "Stop server with signal!");
	delete save;
	exit(0);
}

int	main(int ac, char **av) {
	HttpContext	*webserver;
	bool		error;

	try {
		webserver = new HttpContext();
		save = webserver;
		signal(SIGINT, handler);
		if (ac > 2) 
			throw "Usage: ./webserve [config_file]\n";
		error = webserver->configure(ac == 1 ? "" : std::string(av[1]));
		if (error)
			throw "Wrong config file\n";
		error = webserver->loop();
		if (error)
			throw "Failure during execution\n";// must developpe error
		delete webserver;
	}
	catch(const char* msg) {
		std::cerr << msg << std::endl;
		delete webserver;
	}
	return 0;
}