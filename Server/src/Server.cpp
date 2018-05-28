#include "Server.h"
#include "Utils.h"

#include <boost/bind.hpp>
#include <csignal>
#include <iostream>

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

	Utils::Info << "Server's up." << std::endl;

	acceptor.async_accept(socket,
						  boost::bind(&Server::acceptHandler, this, _1));

	service.run();

	Utils::Info << "Server is down." << std::endl;
}

void Server::acceptHandler(const boost::system::error_code &error) {
	if (error) {
		Utils::Error << "acceptHandler error: " << error << std::endl;
		signalHandler(SIGINT);
		return;
	}

	Utils::Info << "New Client from "
				<< socket.remote_endpoint().address().to_string() << std::endl;

	auto ptr = std::make_shared<Client>(std::move(socket));
	roomless.insert(ptr);

	ptr->on_error = boost::bind(&Server::onError, this, _1);
	ptr->startReceive();

	acceptor.async_accept(socket,
						  boost::bind(&Server::acceptHandler, this, _1));
}

bool Server::onAuth(std::shared_ptr<Client> client) {
	Utils::Success << "Auth: \'" << client->nickname << "\':\'"
				   << client->password << '\'' << std::endl;
	if (true) {
		std::string list = "T";
		for (auto &[name, room] : rooms)
			list += name + ":";

		client->asyncSend(Client::Auth, list);
		return true;
	}
	client->asyncSend(Client::Auth, "F");
	return false;
}

void Server::onError(std::shared_ptr<Client> client) {
	roomless.erase(roomless.find(client));
}

void Server::onRoom(std::shared_ptr<Client> client, std::string room) {
	onError(client);
	rooms[room].add(client);
}