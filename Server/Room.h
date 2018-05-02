#pragma once

#include "Client.h"

#include <list>
#include <string>

class Room {
  private:
	std::list<std::shared_ptr<Client>> clients;

  public:
	Room() {}

	void add(std::shared_ptr<Client>);
	void notifyAll(Client::Event, std::string &);
	void shutdown();
};