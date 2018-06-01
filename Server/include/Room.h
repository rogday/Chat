#pragma once

#include "Client.h"
#include "Messages.h"

#include <cstdint>
#include <memory>
#include <set>
#include <string>

class Room {
  private:
	API::ID id;
	std::set<std::shared_ptr<Client>> clients;

  public:
	Room(API::ID);

	void add(std::shared_ptr<Client>);

	void onRead(std::shared_ptr<Client>, API::Event, std::string &);
	void notifyAll(API::Event, std::string);

	void erase(std::shared_ptr<Client>);
	void shutdown();

	bool operator<(const Room &);
};