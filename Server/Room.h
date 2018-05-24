#pragma once

#include "Client.h"

#include <iostream>
#include <set>
#include <string>

class Room {
  private:
	std::set<std::shared_ptr<Client>> clients;

  public:
	Room() { std::cout << "New instance of Room." << std::endl; }

	void add(std::shared_ptr<Client>);

	void onRead(std::shared_ptr<Client>, std::string &);
	void notifyAll(Client::Event, std::string);

	void erase(std::shared_ptr<Client>);
	void shutdown();
};