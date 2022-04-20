#include "Response.hpp"

Response::Response(const Client& cli, int code) : _client(cli), _code(code)
{ std::cout << "Create Response" << std::endl; }

Response::~Response() {
	std::cout << "Destroy Response" << std::endl;
}
