#include "Client.h"

using namespace boost::asio;

Client Client::client;

void Client::threadfunc() {
	std::string name;

	while (std::getline(std::cin, name))
		asyncSend(Client::Event::ClientAPI, name);
}

void Client::start(char *ip, int port) {
	signal(SIGINT, signalHandler);

	ip::tcp::endpoint ep(ip::address::from_string(ip), port);

	socket.async_connect(
		ep, std::bind(&Client::connect_handler, this, std::placeholders::_1));

	service.run();
}

void Client::connect_handler(const boost::system::error_code &ec) {
	if (ec)
		return;

	std::string nickname, password, room;

	std::cout << "Connected.\nLogin: ";
	std::getline(std::cin, nickname);
	std::cout << "Password: ";
	std::getline(std::cin, password);
	std::cout << "Room name: ";
	std::getline(std::cin, room);

	asyncSend(Auth, nickname + ':' + password);
	asyncSend(Room, room, [this]() {
		std::thread tr(&Client::threadfunc, this);
		tr.detach();
	});

	asyncRecieve();
}

void Client::signalHandler(int n) {
	client.shutdown();

	signal(n, signalHandler);
}

void Client::asyncSend(Event type, std::string str,
					   std::function<void(void)> func) {
	bool empty = msgQueue.empty();

	msgQueue.emplace_back(type, str, func);
	if (empty)
		send();
}

void Client::send() {
	if (!msgQueue.empty()) {
		auto tmp = msgQueue.front();

		writeheader[0] = std::get<1>(tmp).size();
		writeheader[1] = std::get<0>(tmp);
		writebuf = std::get<1>(tmp);
		auto f = std::get<2>(tmp);

		async_write(
			socket,
			boost::asio::buffer((char *)writeheader, sizeof writeheader),
			boost::asio::transfer_exactly(sizeof writeheader),
			[this, f](const boost::system::error_code &err,
					  [[maybe_unused]] size_t n) {
				if (err) {
					std::cerr << "header write error: " << err << std::endl;
					return;
				}

				async_write(socket, boost::asio::buffer(writebuf),
							boost::asio::transfer_exactly(writeheader[0]),
							[this, f](const boost::system::error_code &err,
									  [[maybe_unused]] size_t n) {
								if (err) {
									std::cerr << "content write error: " << err
											  << std::endl;
									return;
								}

								msgQueue.pop_front();

								if (f != nullptr)
									f();

								send();
							});
			});
	}
}

void Client::asyncRecieve() {
	async_read(
		socket, boost::asio::buffer((char *)readheader, sizeof readheader),
		boost::asio::transfer_exactly(sizeof readheader),
		[this](const boost::system::error_code &err,
			   [[maybe_unused]] size_t n) {
			if (err) {
				std::cerr << "header read error in client " << nickname
						  << std::endl;
				return;
			}

			readbuf.resize(readheader[0]);
			async_read(socket, boost::asio::buffer(readbuf),
					   boost::asio::transfer_exactly(readheader[0]),
					   [this](const boost::system::error_code &err,
							  [[maybe_unused]] size_t n) {
						   if (err) {
							   std::cerr << "content read error in client "
										 << nickname << std::endl;
							   return;
						   }

						   if (readheader[1] >= ClientAPI) {
							   // there you can be sure that it's message from
							   // another client and add your own API for files,
							   // music, voice, etc
							   std::cerr << readbuf << std::endl;
						   } else
							   std::cerr
								   << "Unimplemented feature: " << readheader[1]
								   << ' ' << readbuf << std::endl;

						   asyncRecieve();
					   });
		});
}