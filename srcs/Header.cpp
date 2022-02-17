#include "Header.hpp"

std::ostream&	operator<<(std::ostream& out, const Pair& ret) {
	out << "code:" << ret.first;
	out << "\nurl: " << ret.second;
	return out;
}

std::ostream&	operator<<(std::ostream& out, const Socket& sok) {
	out << sok.first << ":" << sok.second;
	return out;
}