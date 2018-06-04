#pragma once

#include "Messages.h"

#include <cstdint>
#include <string>
#include <unordered_set>

struct Account {
	std::string login;
	std::string password;
	API::ID id;
	std::unordered_set<API::ID> rooms;
};