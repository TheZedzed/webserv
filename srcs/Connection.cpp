#include "Connection.hpp"

Connection::Connection(int fd, int type, Client* client) : _fd(fd), _type(type) {
	_data._client = client;
	if (create_timer() == FAILURE)
		throw std::runtime_error("Failure timer create\n");
	arm_timer();
}

Connection::Connection(int fd, int type, const servers_t& servers) : _fd(fd), _type(type)
{ _data._servers = &servers; }

Connection::~Connection() {
	if (_type == CLIENT)
		delete _data._client;
}

int	Connection::get_fd() const
{ return _fd; }

bool	Connection::get_type() const
{ return _type; }

timer_t	Connection::get_timer() const
{ return _timerid; }

const Connection::servers_t&	Connection::get_servers() const
{ return *_data._servers; }

Client*	Connection::get_client()
{ return _data._client; }

bool	Connection::create_timer() {
	struct sigevent	sev;

	bzero(&sev, sizeof(sigevent));
	sev.sigev_notify = SIGEV_SIGNAL;
	sev.sigev_signo = SIGALRM;
	sev.sigev_value.sival_ptr = &_timerid;
	if (timer_create(CLOCK_REALTIME, &sev, &_timerid) == -1)
		return FAILURE;
	return SUCCESS;
}

bool	Connection::arm_timer() {
	struct itimerspec	its;

	bzero(&its, sizeof(itimerspec));
	its.it_value.tv_sec = 10;
	its.it_value.tv_nsec = 1 / 100000000;
	if (timer_settime(_timerid, CLOCK_REALTIME, &its, NULL) == -1)
		return FAILURE;
	return SUCCESS;
}

bool	Connection::send_and_close() {
	Client*	client;
	size_t	found;

	client = _data._client;
	client->process_res();
	send(_fd, client->raw_data.c_str(), client->raw_data.size(), 0); // check error send +
	found = client->raw_data.find("Connection: close");
	if (found != std::string::npos)
		return true;
	arm_timer();
	return false;
}

int	Connection::retrieve_request() {
	Client*	client;
	char	buf[256];
	int		state;
	int		rlen;

	client = _data._client;
	while ((rlen = recv(_fd, buf, 255, 0)) > 0) {
		buf[rlen] = 0;
		client->process_req(str_t(buf, rlen));
		state = client->get_state();
		if ((state & ERROR) || (state & RESPONSE))
			break ;
	}
	if (rlen == 0)
		return DECONNECT;
	return state;
}
