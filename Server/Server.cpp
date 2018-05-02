#include "Server.h"

using namespace boost::asio;

Server Server::server;

Server::Server() : acceptor(service), clients(0) {
	Client::on_auth = boost::bind(&Server::onAuth, this, _1);
	Client::on_room = boost::bind(&Server::onRoom, this, _1);
};

void Server::signalHandler(int n) {
	server.acceptor.close();

	for (auto &room : server.rooms)
		room.second.shutdown();

	for (auto &client : server.roomless)
		client->shutdown();

	signal(n, signalHandler);
};

void Server::startAtPort(int port) {
	signal(SIGINT, signalHandler);

	service.restart();
	ip::tcp::endpoint endpoint(ip::tcp::v4(), port);
	acceptor.open(endpoint.protocol());
	acceptor.bind(endpoint);
	acceptor.listen();

	std::cout << "Server's up." << std::endl;

	socket_ptr socket(new ip::tcp::socket(service));

	startAccept(socket);

	service.run();

	std::cout << "Server is down." << std::endl;
}

void Server::startAccept(socket_ptr socket) {
	acceptor.async_accept(
		*socket, boost::bind(&Server::acceptHandler, this, socket, _1));
}

void Server::acceptHandler(socket_ptr socket,
						   const boost::system::error_code &error) {
	if (error) {
		std::cerr << "acceptHandler error: " << error << std::endl;
		return; // replace that by something meaningful
	}

	std::cout << "New Client from "
			  << socket->remote_endpoint().address().to_string() << std::endl;
	++clients;

	auto ptr = std::make_shared<Client>(socket);
	roomless.insert(ptr);
	ptr->asyncRecieve();

	socket_ptr newSocket(new ip::tcp::socket(service));

	startAccept(newSocket);
}

void Server::onAuth(std::shared_ptr<Client> client) {
	if (true)
		client->setAuth();
	client->asyncRecieve();
}

void Server::onRoom(std::shared_ptr<Client> client) {
	roomless.erase(roomless.find(client));
	rooms[client->getContent()].add(client);
}