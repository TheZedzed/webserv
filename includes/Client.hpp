#pragma once

# ifndef CLIENT_HPP
# define CLIENT_HPP

# include "Server.hpp"
# include "Request.hpp"
# include "Response.hpp"

# define RQLINE		0x00000001 // process rq line --> recv socket
# define HEADER		0x00000010 // process headers --> recv socket
# define BODY		0x00000100 // process body --> recv socket
# define RD_OP		0x00001000 // need syscall read
# define WR_OP		0x00010000 // need syscall write

# define RESPONSE 	0x00100000 // process response --> send socket
# define RES_HEAD	0x01000000 // process response header --> send socket
# define RES_BODY	0x10000000 // process response body --> send socket

/*
** class Client:
** manage incoming connexion
** store fd associated for communication
** store servers asked by the client
*/
class	Client {
	public:
		typedef std::vector<Server*>	servers_t;
		typedef Request::fields_t		fields_t;

		Client(const servers_t& serv);
		~Client();

		int			get_flags() const;
		Request*	get_request();
		Response*	get_response();

		void	set_flag(int bit);
		void	set_request(Request* request);
		void	set_response(Response* rersponse);

		void	process_req(str_t& raw);
		void	process_res(int code);

	private:
		Client();
		Client(const Client&);
		Client&	operator=(const Client&);

		void	_process_sl(str_t& raw);
		void	_process_head(str_t& raw);
		void	_process_body(str_t& raw);
		void	_process_chunk(str_t& raw);
		void	_response_header(int code);

		int			_flags;
		Request*	_request;
		Response*	_response;
		const servers_t	_servers;
};

#endif