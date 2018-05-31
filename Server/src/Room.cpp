#include "Room.h"
#include "Utils.h"

#include <boost/bind.hpp>
#include <iostream>

Room::Room(uint64_t id) : id(id) {
	Utils::Info << "New instance of Room." << std::endl;
}

void Room::add(std::shared_ptr<Client> newcommer) {
	Utils::Success << "Room assignation: \'" << newcommer->account->login
				   << "\' entered in" << id << std::endl;

	// newcommer->on_read = boost::bind(&Room::onRead, this, _1, _2, _3);
	// newcommer->on_error = boost::bind(&Room::erase, this, _1);

	/*
	on_read(static) should look for right room by received id and transfer
	message to it. it = rooms.find(id); if(it) it->onRead(c, t, m);

	on_error(also static) - should erase user from all rooms/roomless set.
	 */

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
	for (auto it : clients) {
		it->on_error = [](auto) {}; // ignore errors
		it->shutdown();
	}
}

bool Room::operator<(const Room &another) { return this->id < another.id; };