#include mixing.hpp

mixing::mixing(int fd, const std::vector<ServerInfo>* infoVirServs) :
		_fd(fd), _infoVirServs(infoVirServs), _request(infoVirServs), _bytesSent() {}

mixing::~mixing() {}

mixing::mixing(const mixing& c) :
		_fd(c._fd), _infoVirServs(c._infoVirServs), 
		_request(c._request), _response(c._response), _bytesSent(c._bytesSent) {}

mixing& mixing::operator=(mixing a)
{
	swap(a, *this);
	return *this;
}

int mixing::getFd() const						{ return _fd; }

Request* mixing::getRequest()	        		{ return &_request; }

Response* mixing::getResponse()               { return &_response; }

size_t mixing::getNbBytesSent()                 { return _bytesSent; }

int mixing::receiveRequest(const char* buffer)
{
	try
	{
		_request += buffer;
		_request.parsingCheck();
	}
    //set respood with status + request
	catch (const StatusLine& staLine)
	{
		printLog(" >> FD " + convertNbToString(getFd()) + ": Request received\n");
		printLog(" >> FD " + convertNbToString(getFd()) + ": URI: " + _request.getPath() + "\n");

        // Constructing response
        _response.setRequest(&_request);
        _response.setStatusLine(staLine);
        _response.setInfoVirtualServs(_infoVirServs);
		_response.fillBuffer();
	}

	return 0;
}

void mixing::updateNbBytesSent(size_t n)
{
    _bytesSent += n;
}
