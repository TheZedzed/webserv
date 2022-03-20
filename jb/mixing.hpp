#ifndef MIXING_HPP
#define MIXING_HPP

#include <iostream>
#include <string>
#include <exception>
#include <queue>

#include "Request.hpp"
#include "Response.hpp"
#include "StatusLine.hpp"

class mixing
{
	private:

		int								_fd;			// File descriptor receiving / sending with the client
		const std::vector<ServerInfo>*	_infoVirServs;	// Servers blocks from config file that match a specific port
		
		Request							_request;		// Object containing the request
		Response            			_response;		// Queue containing the responses created from request object
        size_t                          _bytesSent;

	public:

		mixing(int fd, const std::vector<ServerInfo>* infoVirServs);
		mixing(const mixing& c);
		~mixing();
		mixing& operator=(mixing a);

		int getFd() const;
		Request* getRequest();
		Response* getResponse();
        size_t getNbBytesSent();

		// Add buffer into request, and request will parse the new line delimited
		int receiveRequest(const char* buffer);
	
        void updateNbBytesSent(size_t n);

}; 

#endif