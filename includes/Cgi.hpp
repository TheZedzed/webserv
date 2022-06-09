#pragma once

# ifndef CGI_HPP
# define CGI_HPP

# include "Request.hpp"

/*
** class Cgi:
** format CGI env
** pass requested script to CGI executable
** receive CGI response
** format HTTP response with CGI response
** write to client socket
*/
class	Cgi {
	public:
		typedef std::vector<char *> cgi_env_t;

		Cgi(const Request& req, const str_t& path);
		~Cgi();

		void	build_env(const str_t& route, int socket);
		bool	exec_cgi(const str_t& route);
		bool	treat_cgi_output(str_t &buffer);

	private:
		void	_find_and_fill_req_field(const str_t& to_find, str_t& to_fill);
		void	_fill_remote_vars(int socketfd);
		void	_insert_env_vars(void);

		cgi_env_t		_env;
		const   Request& _request;
		const   str_t& 	_cgi_path;

		int	_in;
		int	_out;

		str_t	_server_name;
		str_t	_server_port;
		str_t	_query_string;
		str_t	_path_info;
		str_t	_document_uri;
		str_t	_request_uri;
		str_t	_path_translated;
		str_t	_script_filename;
		str_t	_script_name;
		str_t	_request_method;
		str_t 	_content_type;
		str_t	_content_length;
		str_t 	_cookie;
		str_t	_remote_port;
		str_t	_remote_address;

		static str_t _outfile_name;
		static str_t _infile_name;
		static str_t _request_scheme;
		static str_t _server_protocol;
		static str_t _gateway_interface;
		static str_t _server_software;
};

#endif
