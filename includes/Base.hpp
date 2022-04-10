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

#define ERR_LOG(code, msg) std::cerr << "Error code: " << code << ", " << msg << std::endl;
#define DEBUG 0
#define SUCCESS 0
#define FAILURE 1

typedef std::string				String;
typedef std::ifstream			Stream;
typedef std::vector<String>		Array;

#endif