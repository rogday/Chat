#pragma once

#include <string>

class Room {
  private:
	std::string name;

  public:
	Room(std::string name) : name(name) {}
};