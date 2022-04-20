#pragma once

# ifndef CLIENT_HPP
# define CLIENT_HPP

# include "Request.hpp"
# include "Response.hpp"

/*
** class Client:
** manage incoming connexion
** store fd associated for communication
** store servers asked by the client
*/
class	Client {
	public:
		Client(int fd, const Event::Servers& serv);
		~Client();

		void	setReq(const String& raw);
		void	setRes(int code);
		bool	sendRes() const;

	private:
		Client();
		Client(const Client&);
		Client&	operator=(const Client&);

		const int	_fd;
		Request*	_request;
		Response*	_response;
		const Event::Servers	_servers;
};

#endif