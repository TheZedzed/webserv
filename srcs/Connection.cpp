#include "Connection.hpp"

Connection::Connection(int fd, int type) : _fd(fd), _type(type)
{}

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

Client*	Connection::getClient()
{ return _data._client; }

int	Connection::getFd() const
{ return _fd; }

bool	Connection::getType() const
{ return _type; }

const Connection::servers_t&	Connection::getServers() const
{ return _data._servers; }

void	Connection::setData(const servers_t& servers)
{ _data._servers = servers; }

void	Connection::setData(Client* client)
{ _data._client = client; }
