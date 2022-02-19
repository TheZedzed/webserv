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

typedef std::string				String;
typedef std::vector<String>		Array;

static String	data_recv;

#endif