#pragma once

#include <algorithm>
#include <cstdint>
#include <string>

namespace API {
using ID = uint64_t;
using Event = uint64_t;
enum { Auth = 0, Room, NewCommer, ClientAPI };

class AuthRequest {
  private:
	std::string buf;
	size_t index;

  public:
	// Client
	AuthRequest(std::string &&, std::string &&);
	std::string &getBuf();

	// Server
	AuthRequest(std::string &&);
	bool isOk();

	std::string getLogin();
	std::string getPassword();
};

class ListPacket {
  private:
	std::string buf;
	size_t index;

  public:
	// Server
	ListPacket(ID);

	void insert(ID, std::string);
	std::string &getBuf();

	// Client
	ListPacket(std::string &&);

	ID getID();
	bool isOk();

	ID getNextID();
	std::string getNextName();
};

using AuthAnswer = ListPacket;
using RoomConnect = ListPacket;

class Message {
  private:
	std::string buf;
	size_t index;

  public:
	// Server
	Message(ID, std::string);
	std::string &getBuf();

	// Client
	Message(std::string);
	ID getID();
	std::string getData();
};

using UserChangedState = Message;

}; // namespace API