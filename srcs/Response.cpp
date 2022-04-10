#include "Response.hpp"

static bool	_wrong_method(const String& elem)
{ return elem != "GET" && elem != "DELETE" && elem != "POST"; }

static bool	_wrong_version(const String& elem)
{ return elem != "HTTP/1.1"; }

Response::~Response()
{ std::cout << "Destroy Response" << std::endl; }

Response::Response()
{ std::cout << "Create an http response" << std::endl; }