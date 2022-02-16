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

#define GET (1 << 1)
#define POST (1 << 2)
#define DELETE (1 << 3)

#define SUCCESS 0
#define FAILURE 1

typedef typename std::string				String;
typedef typename std::vector<String>		Array;
typedef typename std::map<int, String>		ErrPage;
typedef typename std::map<String, String>	Fields;

struct	Redirect {
	String		url;
	uint16_t	code;
};

static String	data_recv;
#endif