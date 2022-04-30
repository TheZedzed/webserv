#pragma once

# ifndef BASE_HPP
# define BASE_HPP

/*
** all necessary dependancies
*/

# include <sys/socket.h>
# include <netinet/in.h>
# include <netinet/ip.h>
# include <arpa/inet.h>
# include <sys/epoll.h>
# include <fcntl.h>
# include <unistd.h>
# include <signal.h>
# include <fstream>
# include <sstream>

# include <iostream>
# include <cstring>
# include <cstdio>
# include <map>
# include <set>
# include <vector>
# include <algorithm>

#define DEBUG 0
#define SUCCESS 0
#define FAILURE 1
#define CRLF "\r\n"
#define SERVER "webserv"

#define CONF_ERR "Wrong config file!"

#define CLIENT 2
#define LISTENNER 3

typedef std::string					str_t;
typedef std::vector<str_t>			strs_t;
typedef std::pair<str_t, str_t>		socket_t;
typedef std::vector<socket_t>		sockets_t;

#endif