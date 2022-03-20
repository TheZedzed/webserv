#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <iostream>
#include <sstream>
#include <string>
#include <cctype>
#include <vector>
#include <map>

#include "StatusLine.hpp"
#include "RequestLine.hpp"
#include "Body.hpp"

class Request
{
	private:

		std::string						_buffer;	
		size_t							_index;		
		const std::vector<ServerInfo>*	_infoVirServs;	// Server blocks from config file that match this port
        
		RequestLine							_reqLine;	// Contains information from the request line
        std::map<std::string, std::string>	_headers;	// Store the headers
        Body								_body;		// Store the body
	
	
	public:

		Request();
		Request(const std::vector<ServerInfo>* infoVirServs);
		Request(const Request& c);
		~Request();
		Request& operator=(Request a);

		const RequestLine& getRequestLine() const;
		const std::map<std::string, std::string>& getHeaders() const;
		const Body& getBody() const;
		const std::string& getBuffer() const;
		int getMethod() const;
		const std::string& getPath() const;
		const std::string& getQuery() const;

        void setPath(const std::string& path);

		// Add the buffer receive->request
		Request& operator+=(const char* charBuffer);

		// check data
		void parsingCheck();

		// Reset request
		void clear();

		// Print Request 
		void print() const;
	
	
	private:

		// Check if a new line was received (request or header).

        bool newLineReceived(size_t posCLRF);

		void parseRequestLine(size_t posCLRF);

		// Checks method
		void parseMethodToken(const std::string& token);

		// Check URI
		void parseURI(std::string token);

		void parseHTTPVersion(const std::string& token);
        
        void parseHeaderLine(size_t posCLRF);
		
		void parseBody();
		
		// Search server block
		long findMaxSize(const std::string& hostName);

		friend void swap(Request& a, Request& b);
		
};

#endif