#pragma once

# ifndef CLIENT_HPP
# define CLIENT_HPP

# include "Server.hpp"
# include "Request.hpp"
# include "Response.hpp"

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

		int			get_state() const;
		Request*	get_request();
		Response*	get_response();

		void	set_state(int state);
		void	set_request(Request* request);
		void	set_response(Response* response);

		const Server*	search_requested_domain() const;

		void	process_req(const str_t& raw);
		void	process_res();
		void	send(int socket);

		str_t	raw_data;

	private:
		Client();
		Client(const Client&);
		Client&	operator=(const Client&);

		int			_state;
		Request*	_request;
		Response*	_response;
		const servers_t	_servers;
};

#endif