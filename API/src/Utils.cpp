#include "Utils.h"

#include <sstream>

std::string Utils::rStr(API::ID n) {
	return std::string(reinterpret_cast<char *>(&n), sizeof n);
}

API::ID Utils::rID(std::string::iterator it) {
	API::ID id;
	std::copy_n(it, sizeof(API::ID), (char *)&id);
	return id;
}

API::ID Utils::toID(std::string s) {
	std::stringstream ss(s);
	API::ID n;
	ss >> n;
	return n;
}