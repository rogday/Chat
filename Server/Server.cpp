#include "Server.h"

using namespace boost::asio;

Server Server::server;

Server::Server() : acceptor(service){};

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
		signalHandler(SIGINT);
		return;
	}

	std::cout << "New Client from "
			  << socket->remote_endpoint().address().to_string() << std::endl;

	auto ptr = std::make_shared<Client>(socket);
	roomless.insert(ptr);

	ptr->on_error = boost::bind(&Server::onError, this, _1);
	ptr->asyncReceive();

	socket_ptr newSocket(new ip::tcp::socket(service));

	startAccept(newSocket);
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

void Server::onRoom(std::shared_ptr<Client> client) {
	onError(client);
	rooms[client->getContent()].add(client);
}