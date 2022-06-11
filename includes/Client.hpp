#pragma once

# ifndef CLIENT_HPP
# define CLIENT_HPP

# include "Parser.hpp"
# include "Request.hpp"
# include "Response.hpp"

class Response;

/*
** class Client:
** manage incoming connection
** save servers which listens on this connection
** format request
** format response
** save current state
*/
class	Client {
	public:
		typedef std::vector<Server*>	servers_t;
		typedef Request::fields_t		fields_t;

		Client(const servers_t& serv);
		~Client();

		void	clear();

		Request*	get_request();
		Response*	get_response();

		void	set_request(Request* request);
		void	set_response(Response* response);

		const Server*	requested_server() const;
		void	process_method(const Location* uri_loc, const str_t& route);
		void	process_response(int& state);

		str_t	raw_data;

	private:
		Client();
		Client(const Client&);
		Client&	operator=(const Client&);

		void	_process_delete(const str_t& route);
		void	_process_redir(const Location* uri_loc);
		void	_process_post(const Location* uri_loc, const str_t& route);
		void	_process_get(const Location* uri_loc, const str_t& route);

		Request*	_request;
		Response*	_response;
		const servers_t	_servers;
};

#endif
