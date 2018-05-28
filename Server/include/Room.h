#pragma once

#include "Client.h"

#include <memory>
#include <set>
#include <string>

class Room {
  private:
	std::set<std::shared_ptr<Client>> clients;

  public:
	Room();

	void add(std::shared_ptr<Client>);

	void onRead(std::shared_ptr<Client>, Client::Event, std::string &);
	void notifyAll(Client::Event, std::string);

	void erase(std::shared_ptr<Client>);
	void shutdown();
};