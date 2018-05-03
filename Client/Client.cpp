#include <boost/asio.hpp>
#include <boost/shared_array.hpp>
#include <iostream>
#include <memory>

using namespace boost::asio;
using socket_ptr =
	std::shared_ptr<ip::tcp::socket>; // try to figure out how to
									  // replace by unique, coz idk

enum Event { Text = 0, Auth, Room, NewCommer };

uint64_t readheader[2];
std::string readbuf;

void asyncSend(socket_ptr sock, Event type, std::string str) {
	boost::shared_array<uint64_t> writeheader(new uint64_t[2]);
	std::shared_ptr<std::string> writebuf(new std::string);

	writeheader[0] = str.size();
	writeheader[1] = type;
	*writebuf = str;

	// capturing buffers prevents them from releasing underlying memory
	async_write(
		*sock, boost::asio::buffer(writeheader.get(), sizeof readheader),
		boost::asio::transfer_exactly(sizeof readheader),
		[sock, writeheader, writebuf](const boost::system::error_code &err,
									  size_t n) {
			if (err) {
				std::cerr << "header write error: " << err << std::endl;
				return;
			}
			std::cout << "Header for " << writeheader[1] << " sent."
					  << std::endl;

			async_write(*sock, boost::asio::buffer(*writebuf),
						boost::asio::transfer_exactly(writeheader[0]),
						[sock, writeheader, writebuf](
							const boost::system::error_code &err, size_t n) {
							if (err) {
								std::cerr << "content write error: " << err
										  << std::endl;
								return;
							}
							std::cout
								<< "Body for " << writeheader[1] << " sent to "
								<< sock->remote_endpoint().address().to_string()
								<< std::endl;
						});
		});
}

void connect_handler(socket_ptr socket, const boost::system::error_code &ec) {
	if (ec) {
		std::cerr << "connect error: " << ec << std::endl;
		return;
	}

	std::cout << "connected" << std::endl;

	asyncSend(socket, Auth, "rogday:password");
	asyncSend(socket, Room, "Motherfuckers");
	asyncSend(socket, Text, "Sup, guys?");
}

int main(int argc, char *argv[]) {
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
