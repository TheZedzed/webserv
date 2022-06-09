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

		void	init();

		int			get_state() const;
		Request*	get_request();
		Response*	get_response();

		void	set_request(Request* request);
		void	set_response(Response* response);

		const Server*	search_requested_domain() const;

		void	process_req(const str_t& raw);
		void	process_res(int fd);
		void	send(int socket);

		str_t	raw_data;

	private:
		Client();
		Client(const Client&);
		Client&	operator=(const Client&);

		bool	_request_time_error();

		int			_state;
		Request*	_request;
		Response*	_response;
		const servers_t	_servers;
};

#endif
