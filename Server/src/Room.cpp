#include "Room.h"
#include "Utils.h"

#include <boost/bind.hpp>
#include <iostream>

Room::Room(API::ID id) : id(id) {
	Utils::Info << "Room#" << id << " is created." << std::endl;
}

void Room::add(std::shared_ptr<Client> newcommer) {
	Utils::Success << "User '" << newcommer->account->login
				   << "' entered in Room#" << id << std::endl;

	API::RoomConnect packet(id);
	for (auto it : clients)
		packet.insert(it->account->id, it->account->login);
	newcommer->asyncSend(API::Room, packet.getBuf());

	API::UserChangedState userState(newcommer->account->id,
									newcommer->account->login);
	notifyAll(API::NewCommer, userState.getBuf());

	clients.insert(newcommer);
}

void Room::onRead(std::shared_ptr<Client> client, API::Event type,
				  std::string &str) {
	API::Message msg(client->account->id, str);
	notifyAll(type, msg.getBuf());
};

void Room::notifyAll(API::Event type, std::string str) {
	API::Message msg(id, str);
	for (auto it : clients)
		it->asyncSend(type, msg.getBuf());
}

void Room::erase(std::shared_ptr<Client> client) {
	clients.erase(clients.find(client)); // careful pls
	API::UserChangedState userState(client->account->id,
									client->account->login);
	notifyAll(API::NewCommer, userState.getBuf());
}

void Room::shutdown() {
	for (auto it : clients)
		it->shutdown();
}

bool Room::operator<(const Room &another) { return this->id < another.id; };