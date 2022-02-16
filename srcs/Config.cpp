#include "Config.hpp"

bool	Config::addSocket(const std::string& address, const std::string& port) {
	std::map<int, std::string>::iterator	it;
	struct sockaddr_in	listen_address;
	int		socket_fd;
	int		res;

	res = 1;
	socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (socket_fd == -1)
		return FAILURE;
	setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &res, sizeof(res));
	memset(&listen_address, 0, sizeof(sockaddr_in));
	listen_address.sin_family = AF_INET; //ipv4
	listen_address.sin_port = htons(std::atoi(port.c_str()));
	listen_address.sin_addr.s_addr = htonl(inet_addr(address.c_str()));
	res = bind(socket_fd, reinterpret_cast<sockaddr *>(&listen_address), sizeof(sockaddr_in));
	if (res == -1)
		return FAILURE;
	res = listen(socket_fd, 100);//up to 100 connections
	if (res == -1)
		return FAILURE;
	_sockets.insert(std::pair<int, std::string>(socket_fd, address + ":" + port));
	return SUCCESS;
}