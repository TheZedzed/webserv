#pragma once

# ifndef LOCATION_HPP
# define LOCATION_HPP

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
#define DEBUG 0
#define SUCCESS 0
#define FAILURE 1

typedef std::string				String;
typedef std::vector<String>		Array;

static String	data_recv;

/* directive location in server context */
class	Location {
	public:
		typedef std::pair<int, String>	Redirect;

		Location();
		~Location();

		void	setRoot(const String& root);
		void	setAutoIndex(const String& autoindex);
		void	setRedirection(const Array& line);
		void	setMethod(const Array& line);
		void	setCgi(const String& line);

		const String&		getRoot() const;
		const Redirect&		getRedir() const;
		bool				getAuto() const;
		const Array&		getAllow() const;
		const String&		getCgi() const;

	private:
		Location(const Location&);
		Location&	operator=(const Location&);

		Array		_allowed; //Sets allowed methods
		Redirect	_redir; // redirection
		bool		_auto; // autoindex
		String		_root; // root
		String		_cgi; // cgi
};

std::ostream&	operator<<(std::ostream& out, const Location& loc);
#endif