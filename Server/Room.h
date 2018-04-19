#pragma once

#include <string>

class Room {
  private:
	// list of users probably?
	// add method maybe?(who is in the room; notify everyone about newcommer)
	// process() or something
	std::string name;

  public:
	Room(std::string name) : name(name) {}
};