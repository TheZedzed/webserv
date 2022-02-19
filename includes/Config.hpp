#pragma once

# ifndef CONFIG_HPP
# define CONFIG_HPP

# include "Location.hpp"

class	Config {
	public:
		typedef std::map<String, Location*>		Routes;
		typedef std::map<int, String>			ErrPage;
		typedef std::pair<String, String>		Socket;
		typedef std::vector<Socket>				Sockets;

		Config();
		~Config();

		void	setErrorPage(const Array& line);
		void	setRoutes(const String& key, Location** loc);
		void	setSockets(const Sockets& sockets);
		void	setNames(const Array& line);
		void	setMax(const String& line);

		const ErrPage&	getErrPages() const;
		const Sockets&	getSockets() const;
		const Routes&	getRoutes() const;
		const Array&	getNames() const;
		size_t			getMax() const;

	private:
		Config(const Config&);
		Config&	operator=(const Config&);

		ErrPage	_error_page; // default: none
		Sockets	_sockets; // vector of pair(host:port)
		Routes	_routes; // default: none
		Array	_names; // default: server_name "";
		size_t	_max; // default: client_max_body_size 1m;
};

std::ostream&	operator<<(std::ostream& out, const Config& config);
std::ostream&	operator<<(std::ostream& out, const Config::Sockets& socks);
std::ostream&	operator<<(std::ostream& out, const Config::Routes& routes);
std::ostream&	operator<<(std::ostream& out, const Config::ErrPage& err);
#endif