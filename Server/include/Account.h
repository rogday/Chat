#pragma once

#include <cstdint>
#include <string>
#include <unordered_set>

struct Account {
	std::string login;
	std::string password;
	uint64_t id;
	std::unordered_set<uint64_t> rooms;
};