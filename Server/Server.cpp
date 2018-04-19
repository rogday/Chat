#include "Server.h"

using namespace boost::asio;

Server Server::server;

void Server::signalHandler(int n) {
	server.acceptor.close();

	for (auto &socket : server.roomless)
		socket->close();

	signal(n, signalHandler);
};

void Server::startAtPort(int port) {
	signal(SIGINT, signalHandler);

	ip::tcp::endpoint endpoint(ip::tcp::v4(), port);
	acceptor.open(endpoint.protocol());
	acceptor.bind(endpoint);
	acceptor.listen();

	std::cout << "Server is up." << std::endl;

	socket_ptr socket(new ip::tcp::socket(service));
	startAccept(socket);
	service.run();

	for (auto &tr : workers)
		tr.join();

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

	++clients;

	roomless.push_back(socket);
	clientSession(socket);

	if (clients > workers.size())
		workers.emplace_back(boost::bind(&io_service::run, &service));

	socket_ptr newSocket(new ip::tcp::socket(service));
	startAccept(newSocket);
}

void Server::readHandler(char *data, socket_ptr socket,
						 const boost::system::error_code &err, std::size_t n) {
	if (err) {
		clientOnError(socket);
		std::cerr << "readHandler error: " << err << std::endl;
		return;
	}

	// std::cout << "read" << std::endl;

	for (int i = 0; i < n; ++i)
		if (data[i] >= 'a' && data[i] <= 'z')
			data[i] += 'A' - 'a';

	socket->async_write_some(
		buffer(data, n),
		boost::bind(&Server::writeHandler, this, data, socket, _1, _2));
};
void Server::writeHandler(char *data, socket_ptr socket,
						  const boost::system::error_code &err, std::size_t n) {
	if (err) {
		clientOnError(socket);
		std::cerr << "writeHandler error: " << err << std::endl;
		return;
	}
	// std::cout << "write" << std::endl;

	socket->async_read_some(
		buffer(data, 512),
		boost::bind(&Server::readHandler, this, data, socket, _1, _2));
};

void Server::clientSession(socket_ptr socket) {
	char *data = new char[512];

	std::cout << "New client: "
			  << socket->remote_endpoint().address().to_string() << std::endl;

	socket->async_read_some(
		buffer(data, 512),
		boost::bind(&Server::readHandler, this, data, socket, _1, _2));

	// cout << "Connection interrupted on client #" << index << endl;
}

void Server::clientOnError(socket_ptr socket) {
	--clients;
	// do not try to access andpoit here, it's not connected,
	// so it will be an exception
};