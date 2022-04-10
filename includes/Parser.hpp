#pragma once

# ifndef PARSER_HPP
# define PARSER_HPP

# include "Event.hpp"
/*
** class Parser:
** parse httpcontext config file (*.conf)
** following mandatory part of the subject
*/
class	Parser {
	public:
		typedef std::map<Config::Socket, Event::Servers>	Listenning;

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

		bool	_empty_line(void);
		void	_fill_map(int flag);

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