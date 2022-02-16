#pragma once

# ifndef CONFIG_HPP
# define CONFIG_HPP

# include "Location.hpp"

class	Config {
	public:
		typedef typename std::map<String, Location*>	Route;

		Config();
		~Config();

		bool	setErrorPage(const Array& line);
		bool	setRoutes(const String& key, Location** loc);
		bool	setSocket(const Array& line);
		bool	setNames(const Array& line);
		bool	setMax(const String& line);

		const ErrPage&	getErrPages() const;
		const Route&	getRoutes() const;
		const Socket&	getSocket() const;
		const Array&	getNames() const;
		const size_t	getMax() const;

	private:
		Config(const Config&);
		Config&	operator=(const Config&);

		ErrPage	_error_page; // default: none
		Socket	_socket; // pair(host:port)
		Route	_routes; // default: none
		Array	_names; // default: server_name "";
		size_t	_max; // default: client_max_body_size 1m;
};

std::ostream&	operator<<(std::ostream& out, const Config& config);
#endif