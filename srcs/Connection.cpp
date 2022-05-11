#include "Connection.hpp"

Connection::Connection(int fd, int type, Client* client) : _fd(fd), _type(type)
{ _data._client = client; }

Connection::Connection(int fd, int type, const servers_t servers) : _fd(fd), _type(type)
{ _data._servers = servers; }

Connection::~Connection() {
	servers_t::iterator	it;

	if (_type == CLIENT)
		delete _data._client;
	else {
		it = _data._servers.begin();
		for (; it != _data._servers.end(); ++it)
			delete *it;
		_data._servers.clear();
	}
}

int	Connection::getFd() const
{ return _fd; }

bool	Connection::getType() const
{ return _type; }

const Connection::servers_t&	Connection::getServers() const
{ return _data._servers; }

Client*	Connection::getClient()
{ return _data._client; }

void	Connection::send(const str_t& data) {
	
}
