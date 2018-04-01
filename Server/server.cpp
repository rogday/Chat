#include <atomic>
#include <boost/asio.hpp>
#include <csignal>
#include <exception>
#include <functional>
#include <iostream>
#include <list>
#include <memory>
#include <thread>
#include <unistd.h>

using namespace boost::asio;
using socket_ptr = std::shared_ptr<ip::tcp::socket>;

void signalHandler(int);
void clientSession(socket_ptr, size_t);
void startAccept(socket_ptr);
void acceptHandler(socket_ptr, const boost::system::error_code &);

io_service service;
ip::tcp::acceptor acceptor(service);
std::atomic<bool> exitServer;

std::list<std::thread> list;

void signalHandler(int sig) {
	exitServer.store(true);
	acceptor.cancel();
}

void startAccept(socket_ptr socket) {
	acceptor.async_accept(
		*socket, std::bind(acceptHandler, socket, std::placeholders::_1));
}

void acceptHandler(socket_ptr socket, const boost::system::error_code &error) {
	static size_t clients = 0;

	if (error) {
		std::cerr << "acceptHandler error: " << error << std::endl;
		return;
	}

	++clients;

	list.push_back(std::thread(clientSession, socket, clients));

	socket_ptr newSocket(new ip::tcp::socket(service));
	startAccept(newSocket);
}

void clientSession(socket_ptr socket, size_t index) {
	char data[512];

	std::cout << "Client #" << index << " connected from "
			  << socket->remote_endpoint().address().to_string() << std::endl;

	while (!exitServer.load()) {
		try {
			size_t len = socket->receive(buffer(data, 512));
			if (len > 0) {
				for (size_t i = 0; i < len; ++i)
					if (data[i] != ' ')
						data[i] += 'A' - 'a';

				socket->send(buffer(data, len));
			}
		} catch (std::exception &e) {
			std::cout << "Client #" << index << " disconnected in cause of \""
					  << e.what() << '"' << std::endl;
			return;
		}
	}

	std::cout << "Connection interrupted on client #" << index << std::endl;
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

	// probably UB here: detached threads checking global variable [fixed]
	// TODO: async read\write, since threads get stiuck on read

	exitServer.store(false);
	std::signal(SIGINT, signalHandler);

	ip::tcp::endpoint endpoint(ip::tcp::v4(), port);
	service.reset();
	acceptor.open(endpoint.protocol());
	acceptor.bind(endpoint);
	acceptor.listen();

	std::cout << "Server is up." << std::endl;

	socket_ptr socket(new ip::tcp::socket(service));
	startAccept(socket);
	service.run();

	for (auto &t : list)
		t.join();

	std::cout << "Server is down." << std::endl;

	return 0;
}