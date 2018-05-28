#include "Server.h"

using namespace boost::asio;

Server Server::server;

Server::Server() : acceptor(service), socket(service){};

void Server::signalHandler(int) {
	server.acceptor.close();

	for (auto &room : server.rooms)
		room.second.shutdown();

	for (auto &client : server.roomless)
		client->shutdown();
};

void Server::startAtPort(int port) {
	signal(SIGINT, signalHandler);

	ip::tcp::endpoint endpoint(ip::tcp::v4(), port);
	socket_base::reuse_address option(true);

	acceptor.open(endpoint.protocol());
	acceptor.set_option(option);
	acceptor.bind(endpoint);
	acceptor.listen();

	std::cout << "Server's up." << std::endl;

	acceptor.async_accept(socket,
						  boost::bind(&Server::acceptHandler, this, _1));

	service.run();

	std::cout << "Server is down." << std::endl;
}

void Server::acceptHandler(const boost::system::error_code &error) {
	if (error) {
		std::cerr << "acceptHandler error: " << error << std::endl;
		signalHandler(SIGINT);
		return;
	}

	std::cout << "New Client from "
			  << socket.remote_endpoint().address().to_string() << std::endl;

	auto ptr = std::make_shared<Client>(std::move(socket));
	roomless.insert(ptr);

	ptr->on_error = boost::bind(&Server::onError, this, _1);
	ptr->asyncReceive();

	acceptor.async_accept(socket,
						  boost::bind(&Server::acceptHandler, this, _1));
}

bool Server::onAuth(std::shared_ptr<Client> client) {
	std::cout << "Auth: \'" << client->nickname << "\':\'" << client->password
			  << '\'' << std::endl;
	if (true) {
		std::string list = "T";
		for (auto &[name, room] : rooms)
			list += name + ":";

		client->asyncSend(Client::Event::Auth, list);
		return true;
	}
	client->asyncSend(Client::Event::Auth, "F");
	return false;
}

void Server::onError(std::shared_ptr<Client> client) {
	roomless.erase(roomless.find(client));
}

void Server::onRoom(std::shared_ptr<Client> client, std::string room) {
	onError(client);
	rooms[room].add(client);
}