#pragma once

# ifndef PARSER_HPP
# define PARSER_HPP

# include "Server.hpp"

/*
** class Parser:
** parse httpcontext config file (*.conf)
** following mandatory part of the subject
*/
class	Parser {
	public:
		typedef std::ifstream					stream_t;
		typedef std::vector<Server*>			servers_t;
		typedef std::map<socket_t, servers_t>	listenners_t;

		Parser(const char* file);
		~Parser();

		const listenners_t&	get_map() const;

		bool	loop(stream_t& in, bool flag);
		void	open_file(const char* file);
		bool	server_directive(stream_t& in, int flag);
		bool	location_directive(stream_t& in, int flag);
		bool	wrong_ldirective(int flag);
		bool	allow_directive(int flag);
		bool	cgi_directive(int flag);
		bool	root_directive(int flag);
		bool	return_directive(int flag);
		bool	autoindex_directive(int flag);
		bool	listen_directive(int flag);
		bool	names_directive(int flag);
		bool	err_page_directive(int flag);
		bool	client_size_directive(int flag);

	private:
		Parser();
		Parser(const Parser&);
		Parser&	operator=(const Parser&);

		void	_new_node(const socket_t socket, Server* server);
		void	_insert_any(const socket_t socket, Server* server);
		bool	_getline(stream_t& in, str_t& buffer);
		void	_fill_map(int flag);
		bool	_server_block();
		bool	_end_of_block();
		socket_t	_lil_dns();

		Server*			_curr_serv;
		Location*		_curr_loc;
		listenners_t	_dumb_map;
		sockets_t		_dumb_tmp; // for checking duplicate sockets
		strs_t			_line;
};

#endif