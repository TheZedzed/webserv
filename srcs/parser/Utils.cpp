#include "Base.hpp"

std::map<int, str_t> code_g;
std::map<int, str_t> page_g;

size_t	_atoi(const str_t& str, int b) {
	std::stringstream	tmp;
	size_t	res(0);

	tmp.str(str);
	(b == 16) ? tmp >> std::hex >> res : tmp >> std::dec >> res;
	return res;
}

str_t	_itoa(int nb) {
	std::stringstream	res;

	res << nb;
	return res.str();
}

str_t&	_tolower(str_t& str) {
	size_t	i = 0;
	while (str[i]) {
		if (str[i] >= 65 && str[i] <= 90)
			str[i] += 32;
		++i;
	}
	return str;
}

void	_init_error_pages() {
	status_msgs();
	default_pages();
}

void status_msgs() {
	code_g[200] = "OK"; // ok
	code_g[301] = "Moved Permanently"; // ok
	code_g[ERR_400] = "Bad Request"; // ok
	code_g[ERR_403] = "Forbidden"; // ok
	code_g[ERR_404] = "Not Found"; // ok
	code_g[ERR_405] = "Method Not Allowed"; // ok
	code_g[ERR_413] = "Payload Too Large"; // ok
	code_g[ERR_414] = "URI Too Long"; // ok
	code_g[ERR_500] = "Internal Server Error"; // ok
	code_g[ERR_501] = "Not Implemented"; // ok
	code_g[ERR_505] = "HTTP Version Not Supported"; // ok
}

void default_pages() {
	page_g[301] =
	"<html>" CRLF
	"<head><title>301 Moved Permanently</title></head>" CRLF
	"<body>" CRLF
	"<center><h1>301 Moved Permanently</h1></center>" CRLF
	"<hr><center>" SERVER "</center>" CRLF
	"</body>" CRLF
	"</html>";

	page_g[400] =
	"<html>" CRLF
	"<head><title>400 Bad Request</title></head>" CRLF
	"<body>" CRLF
	"<center><h1>400 Bad Request</h1></center>" CRLF
	"</body>" CRLF
	"</html>";

	page_g[403] =
	"<html>" CRLF
	"<head><title>403 Forbidden</title></head>" CRLF
	"<body>" CRLF
	"<center><h1>403 Forbidden</h1></center>" CRLF
	"<hr><center>" SERVER "</center>" CRLF
	"</body>" CRLF
	"</html>";

	page_g[404] =
	"<html>" CRLF
	"<head><title>404 Not Found</title></head>" CRLF
	"<body>" CRLF
	"<center><h1>404 Not Found</h1></center>" CRLF
	"<hr><center>" SERVER "</center>" CRLF
	"</body>" CRLF
	"</html>";

	page_g[405] =
	"<html>" CRLF
	"<head><title>405 Method Not Allowed</title></head>" CRLF
	"<body>" CRLF
	"<center><h1>405 Method Not Allowed</h1></center>" CRLF
	"<hr><center>" SERVER "</center>" CRLF
	"</body>" CRLF
	"</html>";

	page_g[413] =
	"<html>" CRLF
	"<head><title>413 Payload Too Large</title></head>" CRLF
	"<body>" CRLF
	"<center><h1>413 Payload Too Large</h1></center>" CRLF
	"<hr><center>" SERVER "</center>" CRLF
	"</body>" CRLF
	"</html>";

	page_g[414] =
	"<html>" CRLF
	"<head><title>414 URI Too Long</title></head>" CRLF
	"<body>" CRLF
	"<center><h1>414 URI Too Long</h1></center>" CRLF
	"<hr><center>" SERVER "</center>" CRLF
	"</body>" CRLF
	"</html>";

	page_g[500] =
	"<html>" CRLF
	"<head><title>500 Internal Server Error</title></head>" CRLF
	"<body>" CRLF
	"<center><h1>500 Internal Server Error</h1></center>" CRLF
	"<hr><center>" SERVER "</center>" CRLF
	"</body>" CRLF
	"</html>";

	page_g[501] =
	"<html>" CRLF
	"<head><title>501 Not Implemented</title></head>" CRLF
	"<body>" CRLF
	"<center><h1>501 Not Implemented</h1></center>" CRLF
	"<hr><center>" SERVER "</center>" CRLF
	"</body>" CRLF
	"</html>";

	page_g[505] =
	"<html>" CRLF
	"<head><title>505 HTTP Version Not Supported</title></head>" CRLF
	"<body>" CRLF
	"<center><h1>505 HTTP Version Not Supported</h1></center>" CRLF
	"<hr><center>" SERVER "</center>" CRLF
	"</body>" CRLF
	"</html>";
}
