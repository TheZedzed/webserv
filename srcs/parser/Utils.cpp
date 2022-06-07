#include "Base.hpp"

std::map<int, str_t>	code_g;
std::map<int, str_t>	page_g;
std::map<str_t, str_t>	mime_g;

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

static void		status_msgs() {
	code_g[200] = "OK"; // ok
	code_g[301] = "Moved Permanently"; // ok
	code_g[400] = "Bad Request"; // ok
	code_g[403] = "Forbidden"; // ok
	code_g[404] = "Not Found"; // ok
	code_g[405] = "Method Not Allowed"; // ok
	code_g[411] = "Length Required"; // ok
	code_g[413] = "Payload Too Large"; // ok
	code_g[414] = "URI Too Long"; // ok
	code_g[500] = "Internal Server Error"; // ok
	code_g[501] = "Not Implemented"; // ok
	code_g[505] = "HTTP Version Not Supported"; // ok
}

static void		default_pages() {
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

	page_g[411] =
	"<html>" CRLF
	"<head><title>411 Length Required</title></head>" CRLF
	"<body>" CRLF
	"<center><h1>411 Length Required</h1></center>" CRLF
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

static void		mimes_types(){
	mime_g["aac"]		= "audio/aac";
	mime_g["mp3"]		= "audio/mp3";
	mime_g["abw"]		= "application/x-abiword";
	mime_g["arc"]		= "application/octet-stream";
	mime_g["avi"]		= "video/x-msvideo";
	mime_g["azw"]		= "application/vnd.amazon.ebook";
	mime_g["bin"]		= "application/octet-stream";
	mime_g["bmp"]		= "image/bmp";
	mime_g["bz"]	 	= "application/x-bzip";
	mime_g["bz2"]		= "application/x-bzip2";
	mime_g["csh"]		= "application/x-csh";
	mime_g["css"]		= "text/css";
	mime_g["csv"]		= "text/csv";
	mime_g["doc"]		= "application/msword";
	mime_g["docx"]	= "application/vnd.openxmlformats-officedocument.wordprocessingml.document";
	mime_g["eot"]		= "application/vnd.ms-fontobject";
	mime_g["epub"]	= "application/epub+zip";
	mime_g["gif"]		= "image/gif";
	mime_g["htm"]		= "text/html";
	mime_g["html"]	= "text/html";
	mime_g["ico"]		= "image/x-icon";
	mime_g["ics"]		= "text/calendar";
	mime_g["jar"]		= "application/java-archive";
	mime_g["jpeg"]	= "image/jpeg";
	mime_g["jpg"]		= "image/jpeg";
	mime_g["js"]		= "application/javascript";
	mime_g["json"]	= "application/json";
	mime_g["mid"]		= "audio/midi";
	mime_g["midi"]	= "audio/midi";
	mime_g["mid"]		= "audio/midi";
	mime_g["mpeg"]	= "video/mpeg";
	mime_g["mpkg"]	= "application/vnd.apple.installer+xml";
	mime_g["odp"]		= "application/vnd.oasis.opendocument.presentation";
	mime_g["ods"]		= "application/vnd.oasis.opendocument.spreadsheet";
	mime_g["odt"]		= "application/vnd.oasis.opendocument.text";
	mime_g["oga"]		= "audio/ogg";
	mime_g["ogv"]		= "video/ogg";
	mime_g["mp4"]		= "video/mp4";
	mime_g["ogx"]		= "application/ogg";
	mime_g["otf"]		= "font/otf";
	mime_g["png"]		= "image/png";
	mime_g["pdf"]		= "application/pdf";
	mime_g["ppt"]		= "application/vnd.ms-powerpoint";
	mime_g["pptx"]	= "application/vnd.openxmlformats-officedocument.presentationml.presentation";
	mime_g["rar"]		= "application/x-rar-compressed";
	mime_g["rtf"]		= "application/rtf";
	mime_g["sh"]		= "application/x-sh";
	mime_g["svg"]		= "image/svg+xml";
	mime_g["swf"]		= "application/x-shockwave-flash";
	mime_g["tar"]		= "application/x-tar";
	mime_g["tif"]		= "image/tiff";
	mime_g["tiff"]	= "image/tiff";
	mime_g["ts"]		= "application/typescript";
	mime_g["ttf"]		= "font/ttf";
	mime_g["vsd"]		= "application/vnd.visio";
	mime_g["wav"]		= "audio/x-wav";
	mime_g["weba"]	= "audio/webm";
	mime_g["webm"]	= "video/webm";
	mime_g["webp"]	= "image/webp";
	mime_g["woff"]	= "font/woff";
	mime_g["woff2"]	= "font/woff2";
	mime_g["xhtml"]	= "application/xhtml+xml";
	mime_g["xls"]		= "application/vnd.ms-excel";
	mime_g["xlsx"]	= "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet";
	mime_g["xml"]		= "application/xml";
	mime_g["xul"]		= "application/vnd.mozilla.xul+xml";
	mime_g["zip"]		= "application/zip";
	mime_g["3gp"]		= "video/3gpp";
	mime_g["3g2"]		= "video/3gpp2";
	mime_g["7z"]		= "application/x-7z-compressed";
}

void	_init() {
	status_msgs();
	default_pages();
	mimes_types();
}
