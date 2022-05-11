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
# include <errno.h>

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

# define LF (u_char)'\n'
# define CR (u_char)'\r'
# define CRLF "\r\n"

# define SERVER "webserv"

# define CONF_ERR "Wrong config file!"

# define CLIENT 2
# define LISTENNER 3

# define RQLINE		0x00000001 // process rq line --> recv socket
# define HEADER		0x00000002 // process headers --> recv socket
# define BODY		0x00000004 // process body --> recv socket
# define RD_OP		0x00000008 // need syscall read
# define WR_OP		0x00000010 // need syscall write
# define RESPONSE 	0x00000020 // process response --> send socket
# define ERROR		0x00000040 // error handler
# define DECONNECT	0x00000080 // client deco

/* error during request time (recv) */
# define ERR_400	0X00000100 // BAD REQUEST
# define ERR_404	0X00000200 // NOT FOUND
# define ERR_501	0X00000400 // NOT IMPLEMENTED
# define ERR_505	0X00000800 // BAD HTTP VERSION

typedef std::string					str_t;
typedef std::vector<str_t>			strs_t;
typedef std::pair<str_t, str_t>		socket_t;
typedef std::vector<socket_t>		sockets_t;

extern	std::map<int, str_t> code_g; // default phrase
extern	std::map<int, str_t> page_g; // defaukt error page

str_t	_itoa(int nb);
void	_init_error_pages();
str_t&	_tolower(str_t& str);
size_t	_atoi(const str_t& str, int b);

#endif