#include "Server.h"

using namespace boost::asio;

Server Server::server;

void Server::signalHandler(int n) {
	server.acceptor.close();

	// for each room - shutdown

	signal(n, signalHandler);
};

void Server::startAtPort(int port) {
	signal(SIGINT, signalHandler);

	ip::tcp::endpoint endpoint(ip::tcp::v4(), port);
	acceptor.open(endpoint.protocol());
	acceptor.bind(endpoint);
	acceptor.listen();

	std::cout << "Server's up." << std::endl;

	socket_ptr socket(new ip::tcp::socket(service));

	acceptor.async_accept(
		*socket, boost::bind(&Server::acceptHandler, this, socket, _1));

	service.run();

	std::cout << "Server is down." << std::endl;
}

void Server::acceptHandler(socket_ptr socket,
						   const boost::system::error_code &error) {
	if (error) {
		std::cerr << "acceptHandler error: " << error << std::endl;
		return; // replace that by something meaningful
	}

	++clients;

	auto ptr = std::make_shared<Client>(socket);
	roomless.insert(ptr);
	ptr->asyncRecieve();

	socket_ptr newSocket(new ip::tcp::socket(service));

	acceptor.async_accept(
		*socket, boost::bind(&Server::acceptHandler, this, socket, _1));
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