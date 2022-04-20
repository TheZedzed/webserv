#include "Base.hpp"

std::map<int, std::string> status_msgs;
std::map<int, std::string> default_pages;

void add_status_msgs() {
	status_msgs[200] = "OK";
	status_msgs[301] = "Moved Permanently"; // return
	status_msgs[400] = "Bad Request"; // ok
	status_msgs[403] = "Forbidden"; // opening / writing / reading
	status_msgs[404] = "Not Found"; // location
	status_msgs[405] = "Method Not Allowed"; // allow
	status_msgs[411] = "Length Required"; // ok
	status_msgs[413] = "Payload Too Large"; // max_body_size
	status_msgs[500] = "Internal Server Error"; // ok
	status_msgs[501] = "Not Implemented"; // ok
	status_msgs[505] = "HTTP Version Not Supported"; // ok
}

void add_default_pages() {

default_pages[301] =
"<html>" CRLF
"<head><title>301 Moved Permanently</title></head>" CRLF
"<body>" CRLF
"<center><h1>301 Moved Permanently</h1></center>" CRLF
"</body>" CRLF
"</html>";

default_pages[400] =
"<html>" CRLF
"<head><title>400 Bad Request</title></head>" CRLF
"<body>" CRLF
"<center><h1>400 Bad Request</h1></center>" CRLF
"</body>" CRLF
"</html>";

default_pages[403] =
"<html>" CRLF
"<head><title>403 Forbidden</title></head>" CRLF
"<body>" CRLF
"<center><h1>403 Forbidden</h1></center>" CRLF
"</body>" CRLF
"</html>";

default_pages[404] =
"<html>" CRLF
"<head><title>404 Not Found</title></head>" CRLF
"<body>" CRLF
"<center><h1>404 Not Found</h1></center>" CRLF
"</body>" CRLF
"</html>";


default_pages[405] =
"<html>" CRLF
"<head><title>405 Not Allowed</title></head>" CRLF
"<body>" CRLF
"<center><h1>405 Not Allowed</h1></center>" CRLF
"</body>" CRLF
"</html>";

default_pages[500] =
"<html>" CRLF
"<head><title>500 Internal Server Error</title></head>" CRLF
"<body>" CRLF
"<center><h1>500 Internal Server Error</h1></center>" CRLF
"</body>" CRLF
"</html>";

default_pages[501] =
"<html>" CRLF
"<head><title>501 Not Implemented</title></head>" CRLF
"<body>" CRLF
"<center><h1>501 Not Implemented</h1></center>" CRLF
"</body>" CRLF
"</html>";

default_pages[505] =
"<html>" CRLF
"<head><title>505 HTTP Version Not Supported</title></head>" CRLF
"<body>" CRLF
"<center><h1>505 HTTP Version Not Supported</h1></center>" CRLF
"</body>" CRLF
"</html>";