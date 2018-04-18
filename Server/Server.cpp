#include "Server.h"

using namespace boost::asio;
using namespace std;
using namespace std::placeholders;

// Server Server::server;
boost::asio::io_service Server::service;
boost::asio::ip::tcp::acceptor Server::acceptor(service);
std::atomic<bool> Server::exitServer;
std::list<socket_ptr> Server::list;

void Server::signalHandler(int n) {
	acceptor.close();
	for (auto &socket : list)
		socket->cancel();
	exitServer.store(true);
	signal(n, signalHandler);
};

void Server::startAtPort(int port) {
	exitServer.store(false);
	signal(SIGINT, signalHandler);

	ip::tcp::endpoint endpoint(ip::tcp::v4(), port);
	service.restart();
	acceptor.open(endpoint.protocol());
	acceptor.bind(endpoint);
	acceptor.listen();

	cout << "Server is up." << endl;

	socket_ptr socket(new ip::tcp::socket(service));
	startAccept(socket);
	service.run();

	// for (auto &t : list)
	//	t.join();

	cout << "Server is down." << endl;
}

void Server::startAccept(socket_ptr socket) {
	acceptor.async_accept(*socket, bind(&Server::acceptHandler, socket, _1));
}

void Server::acceptHandler(socket_ptr socket,
						   const boost::system::error_code &error) {
	static size_t clients = 0;

	if (error) {
		cerr << "acceptHandler error: " << error << endl;
		return;
	}

	++clients;

	list.push_back(socket);
	clientSession(socket, clients);

	socket_ptr newSocket(new ip::tcp::socket(service));
	startAccept(newSocket);
}

void Server::readHandler(char *data, socket_ptr socket,
						 const boost::system::error_code &err, std::size_t n) {
	if (err) {
		cerr << "readHandler error: " << err << endl;
		return;
	}

	cout << "read" << endl;

	for (int i = 0; i < n; ++i)
		if (data[i] >= 'a' && data[i] <= 'z')
			data[i] += 'A' - 'a';

	socket->async_write_some(buffer(data, n),
							 bind(writeHandler, data, socket, _1, _2));
};
void Server::writeHandler(char *data, socket_ptr socket,
						  const boost::system::error_code &err, std::size_t n) {
	if (err) {
		cerr << "writeHandler error: " << err << endl;
		return;
	}
	cout << "write" << endl;

	socket->async_read_some(buffer(data, 512),
							bind(readHandler, data, socket, _1, _2));
};

void Server::clientSession(socket_ptr socket, size_t index) {
	char *data = new char[512];

	cout << "Client #" << index << " connected from "
		 << socket->remote_endpoint().address().to_string() << endl;

	socket->async_read_some(buffer(data, 512),
							bind(readHandler, data, socket, _1, _2));

	// while (!exitServer.load()) {
	//}

	// cout << "Connection interrupted on client #" << index << endl;
}