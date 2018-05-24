#include "Room.h"

void Room::add(std::shared_ptr<Client> newcommer) {
	std::cout << "Room assignation: \'" << newcommer->nickname
			  << "\' entered in \'" << newcommer->getContent() << '\''
			  << std::endl;

	newcommer->on_read = boost::bind(&Room::onRead, this, _1, _2);
	newcommer->on_error = boost::bind(&Room::erase, this, _1);

	std::string online;

	for (auto it : clients)
		online += it->nickname + ":";

	newcommer->asyncSend(Client::Event::Room, online);
	notifyAll(Client::Event::NewCommer, newcommer->nickname);

	clients.insert(newcommer);
}

void Room::onRead(std::shared_ptr<Client> client, std::string &str) {
	notifyAll(client->getType(), client->nickname + "> " + str);
};

void Room::notifyAll(Client::Event type, std::string str) {
	for (auto it : clients)
		it->asyncSend(type, str);
}

void Room::erase(std::shared_ptr<Client> client) {
	clients.erase(clients.find(client)); // careful pls
	notifyAll(Client::Event::NewCommer, client->nickname);
}

void Room::shutdown() {
	for (auto it : clients) {
		it->on_error = [](auto) {}; // ignore errors
		it->shutdown();
	}
}