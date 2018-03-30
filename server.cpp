#include <boost/asio.hpp>
#include <exception>
#include <iostream>
#include <memory>
#include <thread>
#include <unistd.h>

using namespace boost::asio;
using socket_ptr = std::shared_ptr<ip::tcp::socket>;

void clientSession(socket_ptr socket, int index) {
	for (;;) {
		char data[512];
		try {
			size_t len = socket->read_some(buffer(data, 512));
			if (len > 0)
				socket->write_some(buffer(data, len));
		} catch (std::exception &e) {
			std::cout << "Client #" << index << " disconnected in cause of \""
					  << e.what() << '"' << std::endl;
			return;
		}
	}
}

int main(int argc, char *argv[]) {
	if (argc != 2) {
		std::cerr << "Wrong number of parameters!" << std::endl;
		std::cerr << "Try --help." << std::endl;
		return -1;
	}

	if (std::strcmp(argv[1], "--help") == 0) {
		std::cout << "Usage:" << std::endl;
		std::cout << "\tserver port" << std::endl;
		std::cout << "Remember: port range [0-1023] is accessable only by root."
				  << std::endl;
		return 0;
	}

	int port = atoi(argv[1]);

	if (port < 0 || port > 65535) {
		std::cerr << "Wrong port range." << std::endl;
		return -1;
	}

	if (port < 1024 && getuid()) {
		std::cerr << "You must be root to use that port range." << std::endl;
		return -1;
	}

	io_service service;
	ip::tcp::endpoint endpoint(ip::tcp::v4(), port);
	ip::tcp::acceptor acceptor(service, endpoint);

	std::cout << "Server started." << std::endl;

	int clients = 1;
	for (;;) {
		socket_ptr socket(new ip::tcp::socket(service));
		acceptor.accept(*socket);
		std::thread(clientSession, socket, clients).detach();

		std::cout << "Client #" << clients << " connected." << std::endl;
		++clients;
	}

	ip::tcp::socket socket(service);
	socket.connect(endpoint);
	return 0;
}