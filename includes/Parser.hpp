#pragma once

# ifndef PARSER_HPP
# define PARSER_HPP

# include "HttpContext.hpp"

# include <fstream>
# include <sstream>

class	Parser {
	public:
		typedef std::map<Config::Socket, Event::Servers>	Listenning;
		typedef std::ifstream	Stream;

		Parser(const char* file);
		~Parser();

		int					getError() const;
		const Listenning&	getMap() const;

		void	loop(int flag);
		void	open_file(const char* file);
		bool	location_directive(int flag);
		bool	wrong_ldirective(int flag);
		bool	allow_directive(int flag);
		bool	cgi_directive(int flag);
		bool	root_directive(int flag);
		bool	return_directive(int flag);
		bool	autoIndex_directive(int flag);
		bool	wrong_sdirective(int flag);
		bool	listen_directive(int flag);
		bool	names_directive(int flag);
		bool	errPage_directive(int flag);
		bool	clienSize_directive(int flag);

	private:
		Parser();
		Parser(const Parser&);
		Parser&	operator=(const Parser&);

		bool	empty_line(void);
		void	fill_map(int flag);

		int				_error;
		Server*			_curr_serv;
		Config*			_curr_conf;
		Location*		_curr_loc;
		Listenning		_map; // map sockets + servers
		Stream			_in; // filestream
		String			_buffer;
		Array			_line;
		Config::Sockets	_tmp; // save curr_conf sockets
};		
#endif