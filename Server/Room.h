#pragma once

#include "Client.h"

#include <list>
#include <string>

class Room {
  private:
	std::list<std::shared_ptr<Client>> clients;

  public:
	Room() {}
	void add(std::shared_ptr<Client> newcommer) {
		newcommer->on_read = [this](std::shared_ptr<Client> client) {
			notifyAll(Client::Event::Text, client->getContent());
		};
		notifyAll(Client::Event::NewCommer, newcommer->nickname);
		clients.push_back(newcommer);
		newcommer->asyncRecieve();
	}
	void notifyAll(Client::Event type, std::string &str) {
		for (auto it : clients)
			it->asyncSend(type, str);
	}
};