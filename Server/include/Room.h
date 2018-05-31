#pragma once

#include "Client.h"

#include <cstdint>
#include <memory>
#include <set>
#include <string>

class Room {
  private:
	uint64_t id;
	std::set<std::shared_ptr<Client>> clients;

  public:
	Room(uint64_t);

	void add(std::shared_ptr<Client>);

	void onRead(std::shared_ptr<Client>, Client::Event, std::string &);
	void notifyAll(Client::Event, std::string);

	void erase(std::shared_ptr<Client>);
	void shutdown();

	bool operator<(const Room &);
};