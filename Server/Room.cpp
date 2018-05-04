#include "Room.h"

void Room::add(std::shared_ptr<Client> newcommer) {
	std::cout << "Room assignation: \'" << newcommer->nickname
			  << "\' entered in \'" << newcommer->getContent() << '\''
			  << std::endl;

	newcommer->on_read = boost::bind(&Room::onRead, this, _1);
	newcommer->on_error = boost::bind(&Room::erase, this, _1);

	notifyAll(Client::Event::NewCommer, newcommer->nickname);
	clients.insert(newcommer);
}

void Room::onRead(std::shared_ptr<Client> client) {
	// std::cout << "New Message: " << client->getContent() << std::endl;

	notifyAll(client->getType(),
			  client->nickname + "> " + client->getContent());
};

void Room::notifyAll(Client::Event type, std::string str) {
	// std::cout << "Notifying..." << std::endl;
	for (auto it : clients) {
		//	std::cout << "client: " << it->nickname << std::endl;
		it->asyncSend(type, str);
	}
	// std::cout << "The end." << std::endl;
}

void Room::erase(std::shared_ptr<Client> client) {
	clients.erase(clients.find(client));
}

void Room::shutdown() {
	for (auto it : clients)
		it->shutdown();
}