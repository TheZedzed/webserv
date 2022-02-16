#pragma once

# ifndef CONFIG_HPP
# define CONFIG_HPP

# include "Location.hpp"

class	Config {
	public:
		typedef typename std::map<String, Location*>	Locations;

		Config();
		~Config();

		bool	setSocket(const String& address, const String& port);
		bool	setErrorPage(const String& codes, const String& uri);
		bool	setLocations(const String& uri);
		bool	setIndex(const String& indexes);
		bool	setInclude(const String& directory);
		bool	setNames(const String& names);
		bool	setRoot(const String& root);
		bool	setReturn(const String& ret);
		bool	setMaxBodySize(const String& size);

		const Locations	getLocation() const;

	private:
		Config(const Config&);
		Config&	operator=(const Config&);

		Locations	_locations; // default: none
		ErrPage		_error_page; // default: none
		Array		_include; //include file
		Array		_index; // default: index index.html;
		Array		_names; // default: server_name "";
		String		_root; // default: root html;
		Redirect	_ret; // default: none
		size_t		_max; // default: client_max_body_size 1m;
};
#endif