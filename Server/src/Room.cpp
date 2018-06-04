#include "Room.h"
#include "Utils.h"

#include <boost/bind.hpp>
#include <iostream>

Room::Room(API::ID id) : id(id) {
	Utils::Info << "Room#" << id << " is created." << std::endl;
}

void Room::add(std::shared_ptr<Client> newcommer) {
	auto &acc = newcommer->getAcc();

	Utils::Success << "User '" << acc.login << "' entered in Room#" << id
				   << std::endl;

	API::RoomConnect packet(id);
	for (auto it : clients)
		packet.insert(acc.id, acc.login);
	newcommer->asyncSend(API::Room, packet.getBuf());

	API::UserChangedState userState(acc.id, acc.login);
	notifyAll(API::NewCommer, userState.getBuf());

	clients.insert(newcommer);
}

void Room::onRead(std::shared_ptr<Client> client, API::Event type,
				  std::string &str) {
	API::Message msg(client->getAcc().id, str);
	notifyAll(type, msg.getBuf());
};

void Room::notifyAll(API::Event type, std::string str) {
	API::Message msg(id, str);
	for (auto it : clients)
		it->asyncSend(type, msg.getBuf());
}

void Room::erase(std::shared_ptr<Client> client) {
	auto &acc = client->getAcc();
	clients.erase(clients.find(client)); // careful pls
	API::UserChangedState userState(acc.id, acc.login);
	notifyAll(API::NewCommer, userState.getBuf());
}

void Room::shutdown() {
	for (auto it : clients)
		it->shutdown();
}

bool Room::operator<(const Room &another) { return this->id < another.id; };