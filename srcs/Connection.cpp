#include "Connection.hpp"

Connection::Connection(int fd, int type, Client* client) : _state(RQLINE), _fd(fd), _type(type) {
	struct sigevent	sev;

	_data._client = client;
	bzero(&sev, sizeof(sigevent));
	sev.sigev_notify = SIGEV_SIGNAL;
	sev.sigev_signo = SIGALRM;
	sev.sigev_value.sival_ptr = &_timerid;
	if (timer_create(CLOCK_REALTIME, &sev, &_timerid) == -1)
		throw std::runtime_error("Failure timer create\n");
	arm_timer();
}

Connection::Connection(int fd, int type, const servers_t& servers) : _state(-1), _timerid(NULL), _fd(fd), _type(type)
{ _data._servers = &servers; }

Connection::~Connection() {
	if (_type == CLIENT) {
		delete _data._client;
		if (timer_delete(_timerid) == -1)
			throw std::runtime_error("Failure del timer\n");
	}
	close(_fd);
}

int	Connection::get_fd() const
{ return _fd; }

bool	Connection::get_type() const
{ return _type; }

const Connection::servers_t&	Connection::get_servers() const
{ return *_data._servers; }

Client*	Connection::get_client()
{ return _data._client; }

void	Connection::arm_timer() {
	struct itimerspec	its;

	bzero(&its, sizeof(itimerspec));
	its.it_value.tv_sec = 10;
	its.it_value.tv_nsec = 1 / 100000000;
	if (timer_settime(_timerid, CLOCK_REALTIME, &its, NULL) == -1)
		throw std::runtime_error("Failure arm tier!");
}

void	Connection::send_response() {
	Client*	client;
	size_t	found;

	client = _data._client;
	if (!(_state & DECONNECT)) {
		client->process_response(_state);
		send(_fd, client->raw_data.c_str(), client->raw_data.size(), 0);
		found = client->raw_data.find("Connection: close");
		if (found != std::string::npos)
			_state = DECONNECT;
		else
			_state = RESET;
	}
	return ;
}

void	Connection::retrieve_request() {
	Request*	request;
	Client*		client;
	char	buf[256];
	int		rlen;

	client = _data._client;
	request = client->get_request();
	while ((rlen = recv(_fd, buf, 255, 0)) > 0) {
		buf[rlen] = 0;
		client->raw_data.append(str_t(buf, rlen));
		if (_state & RQLINE)
			_state = request->process_rl(client->raw_data);
		if (_state & HEADER)
			_state = request->process_head(client->raw_data);
		if (_state & BODY)
			_state = request->process_body(client->raw_data);
		if (_state & RESPONSE)
			break ;
	}
	if (rlen == 0 && (_state = DECONNECT))
		std::cout << "Client deconnected" << std::endl;
	return ;
}
