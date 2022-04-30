#pragma once

# ifndef CLIENT_HPP
# define CLIENT_HPP

# include "Server.hpp"
# include "Request.hpp"
# include "Response.hpp"

# define RQLINE		0x1 // process rq line --> recv socket
# define HEADER		0x10 // process headers --> recv socket
# define BODY		0x100 // process body --> recv socket
# define RD_OP		0x1000 // need syscall read
# define WR_OP		0x10000 // need syscall write
# define RESPONSE 	0x100000 // process response --> send socket

/*
** class Client:
** manage incoming connexion
** store fd associated for communication
** store servers asked by the client
*/
class	Client {
	public:
		typedef std::vector<Server*>	servers_t;

		Client(const servers_t& serv);
		~Client();

		Request*	getRequest() const;
		Response*	getResponse() const;

		void	process_req(const str_t& raw);
		void	setFlag(int bit);

	private:
		Client();
		Client(const Client&);
		Client&	operator=(const Client&);

		void	_process_sl(const str_t& raw);
		void	_process_head(const str_t& raw);
		void	_process_body(const str_t& raw);
		void	_process_res();

		int			_flags;
		Request*	_request;
		Response*	_response;
		const servers_t	_servers;
};

#endif