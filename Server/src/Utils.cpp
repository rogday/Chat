#include "Utils.h"

std::string Utils::toStr(uint64_t n) {
	return std::string(reinterpret_cast<char *>(&n), sizeof n);
}