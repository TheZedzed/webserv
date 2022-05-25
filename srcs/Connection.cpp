#include "Connection.hpp"

Connection::Connection(int fd, int type, Client* client) : _fd(fd), _type(type)
{ _data._client = client; }

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

const Connection::servers_t&	Connection::get_servers() const
{ return *_data._servers; }

Client*	Connection::get_client()
{ return _data._client; }

bool	Connection::send_and_close() {
	Client*	client;
	//size_t	found;

	client = _data._client;
	client->process_res();
	send(_fd, client->raw_data.c_str(), client->raw_data.size(), 0); // check error send +
	//found = client->raw_data.find("Connection : close");
	//if (found != std::string::npos)
		return true;
	//return false;
}

int	Connection::retrieve_request() {
	Client*	client;
	char	buf[256];
	int		state;
	int		rlen;

	client = _data._client;
	while ((rlen = recv(_fd, buf, 255, 0)) > 0) {
		buf[rlen] = 0;
		client->process_req(buf);
		state = client->get_state();
		if ((state & ERROR) || (state & RESPONSE))
			break ;
	}
	if (rlen == 0)
		return DECONNECT;
	return state;
}
