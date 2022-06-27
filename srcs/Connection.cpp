#include "Connection.hpp"

Connection::Connection(int fd, int type, const servers_t& servers) : _state(RQLINE), _timerid(NULL), _cli(NULL), _type(type), _socket(fd), _servers(servers) {
	std::cout << "Init a new connection!" << std::endl;
	struct sigevent	sev;

	bzero(&sev, sizeof(sigevent));
	if (_type == CLIENT) {
		_cli = new Client();
		sev.sigev_notify = SIGEV_SIGNAL;
		sev.sigev_signo = SIGALRM;
		sev.sigev_value.sival_ptr = &_timerid;
		if (timer_create(CLOCK_REALTIME, &sev, &_timerid) == -1)
			throw std::runtime_error("Failure timer create!");
		arm_timer();
	}
}

Connection::~Connection() {
	std::cout << "Remove a connection!" << std::endl;

	if (_type == CLIENT) {
		delete _cli;
		timer_delete(_timerid);
	}
	close(_socket);
}

const Server*	Connection::_requested_server() const {
	servers_t::const_iterator	it1;
	fields_t::const_iterator	res;
	strs_t::const_iterator		it2;
	str_t	match;

	if (!(_state & ERROR)) {
		it1 = _servers.begin();
		res = _cli->get_request()->get_headers().find("host");
		match = res->second.substr(0, res->second.find_last_of(':')); // host: [name:port]
		for (; it1 != _servers.end(); ++it1) {
			it2 = (*it1)->get_names().begin();
			for (; it2 != (*it1)->get_names().end(); ++it2) {
				if (*it2 == match)
					return *it1;
			}
		}
	}
	return _servers[0];
}

void	Connection::arm_timer() {
	struct itimerspec	its;

	bzero(&its, sizeof(itimerspec));
	its.it_value.tv_sec = 65;
	its.it_value.tv_nsec = 1000 / 100000000;
	if (timer_settime(_timerid, CLOCK_REALTIME, &its, NULL) == -1)
		throw std::runtime_error("Failure arm timer!");
}

void	Connection::send_response() {
	size_t	found1;
	size_t	found2;
	ssize_t	res;

	_cli->set_server(_requested_server());
	_cli->process_response(_socket, _state);
	res = send(_socket, _cli->raw_data.c_str(), _cli->raw_data.size(), 0);
	found1 = _cli->raw_data.find("Connection:");
	if (res != -1 && found1 != std::string::npos) {
		found2 = _cli->raw_data.find("Connection: Keep-alive", found1);
		if (found2 != std::string::npos) {
			_state = RESET;
			_cli->reset();
			return ;
		}
	}
	_state = DECONNECT;
	return ;
}

void	Connection::retrieve_request() {
	char	buf[4096];
	int		rlen;

	std::cout << "address of state " << &_state << "\n";
	rlen = recv(_socket, buf, 4095, 0);
	if (rlen <= 0) {
		if (rlen == 0)
			std::cout << "Client deconnected" << std::endl;
		else
			std::cout << "Error rlen" << std::endl;
		_state = DECONNECT;
	}
	else {
		buf[rlen] = 0;
		_cli->raw_data.append(str_t(buf, rlen));
		_cli->process_request(_state);
	}
	return ;
}
