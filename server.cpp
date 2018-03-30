#include <boost/asio.hpp>
#include <exception>
#include <iostream>
#include <memory>
#include <thread>

using namespace boost::asio;
using socket_ptr = std::shared_ptr<ip::tcp::socket>;

void clientSession(socket_ptr socket) {
	for (;;) {
		char data[512];
		try {
			size_t len = socket->read_some(buffer(data, 512));
			if (len > 0)
				socket->write_some(buffer(data, len));
		} catch (std::exception &e) {
			std::cout << e.what() << std::endl;
			return;
		}
	}
}

int main() {
	io_service service;
	ip::tcp::endpoint endpoint(ip::tcp::v4(), 6666);
	ip::tcp::acceptor acceptor(service, endpoint);

	for (;;) {
		socket_ptr socket(new ip::tcp::socket(service));
		acceptor.accept(*socket);
		std::thread(clientSession, socket).detach();
	}

	ip::tcp::socket socket(service);
	socket.connect(endpoint);
	return 0;
}