#pragma once

# ifndef CONFIG_HPP
# define CONFIG_HPP

# include "Location.hpp"

class	Config {
	public:
		bool	addSocket(const std::string& address, const std::string& port);
		bool	addErrorPage(const std::string& codes, const std::string& uri);
		bool	addLocations(const std::string& uri);
		bool	addIndex(const std::string& indexes);
		bool	addInclude(const std::string& directory);
		bool	addNames(const std::string& names);
		bool	addRoot(const std::string& root);
		bool	addReturn(const std::string& ret);
		bool	addMaxBodySize(const std::string& size);

		std::map<std::string, Location*>	_locations; // default: none
		std::map<int, std::string>			_error_page; // default: none
		std::map<int, std::string>			_sockets; // default: listen *:80 | *:8000; Contain socket (listen_fd, pair(address, port))
		std::vector<std::string>			_include; //include file
		std::vector<std::string>			_index; // default: index index.html;
		std::vector<std::string>			_names; // default: server_name "";
		std::string							_root; // default: root html;
		t_redirect							_ret; // default: none
		uint64_t							_max; // default: client_max_body_size 1m;
};
#endif