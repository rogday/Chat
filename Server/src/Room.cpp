#include "Room.h"
#include "Utils.h"

#include <boost/bind.hpp>
#include <iostream>

Room::Room() { Utils::Info << "New instance of Room." << std::endl; }

void Room::add(std::shared_ptr<Client> newcommer) {
	Utils::Success << "Room assignation: \'" << newcommer->nickname
				   << "\' entered somewhere" << std::endl;

	newcommer->on_read = boost::bind(&Room::onRead, this, _1, _2, _3);
	newcommer->on_error = boost::bind(&Room::erase, this, _1);

	std::string online;

	for (auto it : clients)
		online += it->nickname + ":";

	newcommer->asyncSend(Client::Room, online);
	notifyAll(Client::NewCommer, newcommer->nickname);

	clients.insert(newcommer);
}

void Room::onRead(std::shared_ptr<Client> client, Client::Event type,
				  std::string &str) {
	notifyAll(type, client->nickname + "> " + str);
};

void Room::notifyAll(Client::Event type, std::string str) {
	for (auto it : clients)
		it->asyncSend(type, str);
}

void Room::erase(std::shared_ptr<Client> client) {
	clients.erase(clients.find(client)); // careful pls
	notifyAll(Client::NewCommer, client->nickname);
}

void Room::shutdown() {
	for (auto it : clients) {
		it->on_error = [](auto) {}; // ignore errors
		it->shutdown();
	}
}