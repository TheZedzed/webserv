#pragma once

# ifndef LOCATION_HPP
# define LOCATION_HPP

# include "Header.hpp"

/* directive location in server context */
class	Location {
	public:
		Location();//default value from nginx
		bool	setBodySize(std::string);
		bool	setRoot(std::string);
		bool	setIndex(std::string);
		bool	setErrorPages(std::string);
		bool	setAutoIndex(std::string);
		bool	setRedirection(std::string);
		bool	setMethod(std::string);
		bool	setCGIpass(std::string);
		bool	setCGIparam(std::string);
	private:
		uint32_t					_client_max_body_size;//maximum allowed size of the client request body. If the size in a >>, the 413 (Request Entity Too Large) is send
		std::string					_root; //Sets the root directory for requests
		std::set<std::string>		_index; //requests ending with the slash character (‘/’)
		std::map<int, std::string>	_error_page;// error pages
		bool						_autoindex; // requests ending with the slash character (‘/’) and produces a directory listing when cant find index
		t_redirect					_return; //http redirection
		uint8_t						_allowed; //Sets allowed methods
		std::string					_fastcgi_pass; //Sets the address of a FastCGI server
		std::string					_fastcgi_param ; //Sets a parameter that should be passed to the FastCGI server
};
#endif