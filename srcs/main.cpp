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
			throw "Usage: ./webserve [config_file]";
		error = webserver->configure(ac == 1 ? "" : String(av[1]));
		if (error)
			throw "Wrong config file";
		error = webserver->loop();
		if (error)
			throw "Failure during execution";// must developpe FAILURE
		delete webserver;
	}
	catch(const char* msg) {
		std::cerr << msg << std::endl;
		delete webserver;
	}
	return 0;
}