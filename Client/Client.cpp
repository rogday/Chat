#include "Client.h"

using namespace boost::asio;

Client Client::client;

void Client::connectToServer(std::string ip, int port) {
	signal(SIGINT, signalHandler);

	ip::tcp::endpoint ep(ip::address::from_string(ip), port);

	socket.async_connect(
		ep, std::bind(&Client::connect_handler, this, std::placeholders::_1));
}

void Client::connect_handler(const boost::system::error_code &ec) {
	if (ec) {
		std::cout << "connect error: " << ec << std::endl;
		on_error();
	} else {
		login();
		startRecieving();
	}
}

void Client::signalHandler(int n) {
	client.shutdown();

	signal(n, signalHandler);
}

void Client::asyncSend(Event type, std::string str) {
	bool empty = msgQueue.empty();

	msgQueue.emplace_back(type, str);
	if (empty)
		send();
}

void Client::send() {
	if (!msgQueue.empty()) {
		auto tmp = msgQueue.front();

		writeheader[0] = std::get<1>(tmp).size();
		writeheader[1] = std::get<0>(tmp);
		writebuf = std::get<1>(tmp);

		async_write(
			socket,
			boost::asio::buffer((char *)writeheader, sizeof writeheader),
			boost::asio::transfer_exactly(sizeof writeheader),
			[this](const boost::system::error_code &err,
				   [[maybe_unused]] size_t n) {
				if (err) {
					std::cerr << "header write error: " << err << std::endl;
					on_error();
					return;
				}

				async_write(socket, boost::asio::buffer(writebuf),
							boost::asio::transfer_exactly(writeheader[0]),
							[this](const boost::system::error_code &err,
								   [[maybe_unused]] size_t n) {
								if (err) {
									std::cerr << "content write error: " << err
											  << std::endl;
									on_error();
									return;
								}

								msgQueue.pop_front();
								send();
							});
			});
	}
}

void Client::startRecieving() {
	async_read(socket,
			   boost::asio::buffer((char *)readheader, sizeof readheader),
			   boost::asio::transfer_exactly(sizeof readheader),
			   [this](const boost::system::error_code &err,
					  [[maybe_unused]] size_t n) {
				   if (err) {
					   std::cerr << "header read error: " << err << std::endl;
					   on_error();
					   return;
				   }

				   readbuf.resize(readheader[0]);
				   async_read(socket, boost::asio::buffer(readbuf),
							  boost::asio::transfer_exactly(readheader[0]),
							  [this](const boost::system::error_code &err,
									 [[maybe_unused]] size_t n) {
								  if (err) {
									  std::cerr << "content read error: " << err
												<< std::endl;
									  on_error();
									  return;
								  }

								  if (readheader[1] >= ClientAPI) {
									  on_read(readbuf);
									  // there you can be sure that it's message
									  // from another client and add your own
									  // API for files, music, voice, etc
								  } else
									  switch (readheader[1]) {
									  case Auth:
										  if (readbuf[0] == 'F')
											  login();
										  else {
											  on_auth(readbuf.substr(1));
										  }
										  break;
									  case Room:
										  on_room(readbuf);
										  break;
									  case NewCommer:
										  on_newcommer(readbuf);
										  break;
									  default:
										  std::cerr << "Unimplemented feature: "
													<< readheader[1] << ' '
													<< readbuf << std::endl;
									  }

								  startRecieving();
							  });
			   });
}
