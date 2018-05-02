#include "Room.h"

void Room::add(std::shared_ptr<Client> newcommer) {
	std::cout << "Newcommer in room \'" << newcommer->getContent() << "\': \'"
			  << newcommer->nickname << '\'' << std::endl;
	newcommer->on_read = [this](std::shared_ptr<Client> client) {
		std::cout << "New Message: " << client->getContent() << std::endl;
		notifyAll(Client::Event::Text, client->getContent());
	};
	notifyAll(Client::Event::NewCommer, newcommer->nickname);
	clients.push_back(newcommer);
	newcommer->asyncReceive();
}

void Room::notifyAll(Client::Event type, std::string &str) {
	for (auto &it : clients)
		it->asyncSend(type, str);
}

void Room::shutdown() {
	for (auto &it : clients)
		it->shutdown();
}