#include "Utils.h"

#include <sstream>

std::string Utils::toStr(uint64_t n) {
	return std::string(reinterpret_cast<char *>(&n), sizeof n);
}

uint64_t Utils::toU64(std::string s) {
	std::stringstream ss(s);
	uint64_t n;
	ss >> n;
	return n;
}