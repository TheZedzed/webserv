#include "Header.hpp"

std::ostream&	operator<<(std::ostream& out, const Redirect& ret) {
	out << "\ncode:" << ret.code;
	out << "\nurl: " << ret.url;
	return out;
}