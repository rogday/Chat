#include <boost/asio.hpp>
#include <chrono>
#include <iostream>
#include <memory>
#include <random>
#include <thread>

using namespace boost::asio;
using socket_ptr =
	std::shared_ptr<ip::tcp::socket>; // try to figure out how to
									  // replace by unique, coz idk

void connect_handler(socket_ptr socket, const boost::system::error_code &ec) {
	if (ec)
		return;

	char data[512]{};

	while (true) {
		try {
			size_t len = 1 + rand() % 150;

			std::generate(std::begin(data), std::begin(data) + len,
						  []() -> char {
							  if (rand() % 7 != 0)
								  return 'a' + rand() % 26;
							  else
								  return ' ';
						  }); // I'm kidding, that's awful to use.

			socket->send(buffer(data, len));

			std::cout << "Sent:" << std::endl;

			for (size_t i = 0; i < len; ++i)
				std::cout << data[i];
			std::cout << std::endl;

			len = socket->receive(buffer(data, 512));

			std::cout << "Received:" << std::endl;

			for (size_t i = 0; i < len; ++i)
				std::cout << data[i];
			std::cout << std::endl << std::endl;

			using namespace std::chrono_literals;
			// std::this_thread::sleep_for(2s);

		} catch (std::exception &e) {
			std::cout << "Something went wrong: " << e.what() << std::endl;
			return;
		}
	}
}

int main(int argc, char *argv[]) {
	srand(time(nullptr));
	/*Help, number of args, range, etc checks like in server*/
	char *ip = argv[1];
	int port = atoi(argv[2]);

	io_service service;
	ip::tcp::endpoint ep(ip::address::from_string(ip), port);
	socket_ptr socket(new ip::tcp::socket(service));

	socket->async_connect(
		ep, std::bind(connect_handler, socket, std::placeholders::_1));
	service.run();

	std::cout << "dying" << std::endl;

	return 0;
}
