#include "Room.h"
#include "Utils.h"

#include <boost/bind.hpp>
#include <iostream>

Room::Room(uint64_t id) : id(id) {
	Utils::Info << "Room#" << id << " is created." << std::endl;
}

void Room::add(std::shared_ptr<Client> newcommer) {
	Utils::Success << "User '" << newcommer->account->login
				   << "' entered in Room#" << id << std::endl;

	std::string online = Utils::toStr(id); // add offline users too
	for (auto it : clients)
		online += Utils::toStr(it->account->id) + it->account->login + ":";

	newcommer->asyncSend(Client::Room, online);
	notifyAll(Client::NewCommer,
			  Utils::toStr(newcommer->account->id) + newcommer->account->login);

	clients.insert(newcommer);
}

void Room::onRead(std::shared_ptr<Client> client, Client::Event type,
				  std::string &str) {
	notifyAll(type, Utils::toStr(client->account->id) + str);
};

void Room::notifyAll(Client::Event type, std::string str) {
	for (auto it : clients)
		it->asyncSend(type, Utils::toStr(id) + str);
}

void Room::erase(std::shared_ptr<Client> client) {
	clients.erase(clients.find(client)); // careful pls
	notifyAll(Client::NewCommer,
			  Utils::toStr(client->account->id) + client->account->login);
}

void Room::shutdown() {
	for (auto it : clients)
		it->shutdown();
}

bool Room::operator<(const Room &another) { return this->id < another.id; };