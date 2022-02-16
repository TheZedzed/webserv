#pragma once

# ifndef LOCATION_HPP
# define LOCATION_HPP

# include "Header.hpp"

/* directive location in server context */
class	Location {
	public:
		Location();//default value from nginx
		~Location();

		bool	setBodySize(String);
		bool	setRoot(String);
		bool	setIndex(String);
		bool	setErrorPages(String);
		bool	setAutoIndex(String);
		bool	setRedirection(String);
		bool	setMethod(String);
		bool	setCGIpass(String);
		bool	setCGIparam(String);

	private:
		Location(const Location&);
		Location&	operator=(const Location&);

		uint32_t	_max;//maximum allowed size of the client request body. If the size in a >>, the 413 (Request Entity Too Large) is send
		String		_root; //Sets the root directory for requests
		Array		_index; //requests ending with the slash character (‘/’)
		ErrPage		_error_page;// error pages
		bool		_autoindex; // requests ending with the slash character (‘/’) and produces a directory listing when cant find index
		Redirect	_ret; //http redirection
		uint8_t		_allowed; //Sets allowed methods
		String		_fastcgi_pass; //Sets the address of a FastCGI server
		String		_fastcgi_param ; //Sets a parameter that should be passed to the FastCGI server
};
#endif