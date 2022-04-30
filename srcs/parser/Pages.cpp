#include "Base.hpp"

std::map<int, std::string> code;
std::map<int, std::string> page;

void status_msgs() {
	code[200] = "OK";
	code[301] = "Moved Permanently"; // return
	code[400] = "Bad Request"; // ok
	code[403] = "Forbidden"; // opening / writing / reading
	code[404] = "Not Found"; // location
	code[405] = "Method Not Allowed"; // allow
	code[411] = "Length Required"; // ok
	code[413] = "Payload Too Large"; // max_body_size
	code[414] = "URI Too Long"; // ok
	code[500] = "Internal Server Error"; // ok
	code[501] = "Not Implemented"; // ok
	code[505] = "HTTP Version Not Supported"; // ok
}

void default_pages() {
	page[301] =
	"<html>" CRLF
	"<head><title>301 Moved Permanently</title></head>" CRLF
	"<body>" CRLF
	"<center><h1>301 Moved Permanently</h1></center>" CRLF
	"<hr><center>" SERVER "</center>" CRLF
	"</body>" CRLF
	"</html>";

	page[400] =
	"<html>" CRLF
	"<head><title>400 Bad Request</title></head>" CRLF
	"<body>" CRLF
	"<center><h1>400 Bad Request</h1></center>" CRLF
	"</body>" CRLF
	"</html>";

	page[403] =
	"<html>" CRLF
	"<head><title>403 Forbidden</title></head>" CRLF
	"<body>" CRLF
	"<center><h1>403 Forbidden</h1></center>" CRLF
	"<hr><center>" SERVER "</center>" CRLF
	"</body>" CRLF
	"</html>";

	page[404] =
	"<html>" CRLF
	"<head><title>404 Not Found</title></head>" CRLF
	"<body>" CRLF
	"<center><h1>404 Not Found</h1></center>" CRLF
	"<hr><center>" SERVER "</center>" CRLF
	"</body>" CRLF
	"</html>";

	page[405] =
	"<html>" CRLF
	"<head><title>405 Not Allowed</title></head>" CRLF
	"<body>" CRLF
	"<center><h1>405 Not Allowed</h1></center>" CRLF
	"<hr><center>" SERVER "</center>" CRLF
	"</body>" CRLF
	"</html>";

	page[500] =
	"<html>" CRLF
	"<head><title>500 Internal Server Error</title></head>" CRLF
	"<body>" CRLF
	"<center><h1>500 Internal Server Error</h1></center>" CRLF
	"<hr><center>" SERVER "</center>" CRLF
	"</body>" CRLF
	"</html>";

	page[501] =
	"<html>" CRLF
	"<head><title>501 Not Implemented</title></head>" CRLF
	"<body>" CRLF
	"<center><h1>501 Not Implemented</h1></center>" CRLF
	"<hr><center>" SERVER "</center>" CRLF
	"</body>" CRLF
	"</html>";

	page[505] =
	"<html>" CRLF
	"<head><title>505 HTTP Version Not Supported</title></head>" CRLF
	"<body>" CRLF
	"<center><h1>505 HTTP Version Not Supported</h1></center>" CRLF
	"<hr><center>" SERVER "</center>" CRLF
	"</body>" CRLF
	"</html>";
}