#pragma once

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

using Error = Colored<91>;
using Info = Colored<37>;
using Success = Colored<92>;
} // namespace

static Error Error;
static Info Info;
static Success Success;

std::string toStr(uint64_t);
}; // namespace Utils