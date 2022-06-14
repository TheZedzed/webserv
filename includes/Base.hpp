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
# include <sys/types.h>
# include <sys/wait.h>
# include <sys/stat.h>
# include <unistd.h>
# include <signal.h>
# include <errno.h>
# include <fcntl.h>

# include <map>
# include <ctime>
# include <vector>
# include <fstream>
# include <sstream>
# include <cstring>
# include <iostream>
# include <algorithm>

# define DEBUG 1

# define SUCCESS 0
# define FAILURE 1

# define CLIENT 1
# define SERVERS 0

# define LF (u_char)'\n'
# define CR (u_char)'\r'
# define CRLF "\r\n"

# define SERVER "webserv"

# define RQLINE		0x00000001 // process rq line --> recv socket
# define HEADER		0x00000002 // process headers --> recv socket
# define BODY		0x00000004 // process body --> recv socket
# define RESPONSE 	0x00000008 // process response --> send socket
# define ERROR		0x00000010 // error handler
# define DECONNECT	0x00000020 // client deconnection
# define RESET		0x00000040 // reset connection

# define ERR_200	0x00000080
# define ERR_301	0x00000100
# define ERR_400	0X00000200
# define ERR_403	0X00000400
# define ERR_404	0X00000800
# define ERR_405	0X00001000
# define ERR_413	0X00002000
# define ERR_414	0X00004000
# define ERR_500	0X00008000
# define ERR_501	0X00010000
# define ERR_505	0X00020000

typedef std::string				str_t;
typedef std::vector<str_t>		strs_t;
typedef std::map<str_t, str_t>	fields_t;
typedef std::pair<str_t, str_t>	socket_t;
typedef std::vector<socket_t>	sockets_t;

extern std::map<int, str_t>		code_g; // default phrase
extern std::map<int, str_t>		page_g; // default error page
extern std::map<str_t, str_t>	mime_g; // mimes type

void	_init();
str_t	_itoa(int nb);
int		_translate(int code);
str_t&	_tolower(str_t& str);
size_t	_atoi(const str_t& str, int b);

#endif
