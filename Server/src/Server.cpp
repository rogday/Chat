#include "Server.h"
#include "Utils.h"

#include <boost/bind.hpp>
#include <csignal>
#include <iostream>
#include <thread>

using namespace boost::asio;

Server Server::server;

Server::Server() : acceptor(service), socket(service), database(service){};

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
		Utils::Error << "Accept error: " << error << std::endl;
		signalHandler(SIGINT);
		return;
	}

	Utils::Info << "New Client from "
				<< socket.remote_endpoint().address().to_string() << std::endl;

	auto ptr = std::make_shared<Client>(std::move(socket));
	roomless.insert(ptr);
	ptr->startReceive();

	acceptor.async_accept(socket,
						  boost::bind(&Server::acceptHandler, this, _1));
}

void Server::onAuth(std::shared_ptr<Client> client, std::string login,
					std::string password) {
	std::thread t(&DB::getUserInfo, &database, login, password,
				  [this, client](API::AuthAnswer answ, Account acc) {
					  client->setAuth(acc);

					  Utils::Success << "Auth: '" << acc.login << "':'"
									 << acc.password << "'" << std::endl;

					  client->asyncSend(API::Auth, answ.getBuf());

					  if (!acc.rooms.empty())
						  roomless.erase(roomless.find(client));

					  for (API::ID room_id : acc.rooms) {
						  auto it = rooms.find(room_id);
						  if (it == rooms.end())
							  it = rooms.emplace(room_id, Room(room_id)).first;
						  it->second.add(client);
					  }
				  },
				  [client]() {
					  Utils::Error << "Client " << client.get()
								   << " failed to log in." << std::endl;
					  client->asyncSend(API::Auth);
				  });
	t.detach();
}

void Server::onRead(std::shared_ptr<Client> client, API::Event type,
					API::ID room_id, std::string mes) {
	rooms.find(room_id)->second.onRead(client, type, mes);
}

void Server::onError(std::shared_ptr<Client> client) {
	auto &acc = client->getAcc();
	if (acc.login.empty() || acc.rooms.empty())
		roomless.erase(roomless.find(client));
	else
		for (API::ID room_id : acc.rooms)
			rooms.find(room_id)->second.erase(client);
}

bool Server::onRoom(std::shared_ptr<Client> client, API::ID room_id) {
	auto &acc = client->getAcc();
	if (database.mayConnect(acc.id, room_id)) {
		if (acc.rooms.empty())
			roomless.erase(roomless.find(client));

		auto it = rooms.find(room_id);
		if (it == rooms.end())
			it = rooms.emplace(room_id, Room(room_id)).first;

		it->second.add(client);
		return true;
	}
	return false;
}