#pragma once

#include "Messages.h"

#include <cstdint>
#include <iostream>
#include <string>

namespace Utils {

namespace {
template <size_t Color> class Colored {
  public:
	template <class T> Colored &operator<<(const T &mes) {
		std::cerr << "\e[" << Color << 'm' << mes;
		return *this;
	}

	Colored &operator<<(std::ostream &(*)(std::ostream &)) {
		std::cerr << "\e[39m" << std::endl;
		return *this;
	}
};

using err = Colored<91>;
using info = Colored<37>;
using succ = Colored<92>;
} // namespace

static err Error;
static info Info;
static succ Success;

std::string rStr(API::ID);
API::ID rID(std::string::iterator);

API::ID toID(std::string s);
}; // namespace Utils