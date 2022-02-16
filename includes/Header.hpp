#pragma once

# ifndef HEADER_HPP
# define HEADER_HPP

# include <sys/socket.h>
# include <netinet/in.h>
# include <netinet/ip.h>
# include <arpa/inet.h>
# include <sys/epoll.h>
# include <fcntl.h>
# include <unistd.h>
# include <signal.h>

# include <iostream>
# include <cstring>
# include <cstdio>
# include <map>
# include <set>
# include <vector>
# include <algorithm>

#define ERR_LOG(code, msg) std::cerr << "Error code: " << code << ", " << msg << std::endl;

#define SUCCESS 0
#define FAILURE 1

typedef typename std::string				String;
typedef typename std::vector<String>		Array;
typedef typename std::map<int, String>		ErrPage;
typedef typename std::map<String, String>	Fields;
typedef typename std::pair<int, String>		Pair;
typedef typename std::pair<String, String>	Socket;

static String	data_recv;

std::ostream&	operator<<(std::ostream& out, const Pair& ret);
std::ostream&	operator<<(std::ostream& out, const Socket& sok);
#endif