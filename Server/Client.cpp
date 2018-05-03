#include "Client.h"

// std::function<void(std::shared_ptr<Client>)> Client::on_auth = nullptr;
// std::function<void(std::shared_ptr<Client>)> Client::on_room = nullptr;

void Client::asyncSend(Event type, std::string str) {
	bool empty = msgQueue.empty();

	msgQueue.emplace_back(type, str);
	if (empty)
		send();
}

void Client::send() {
	if (!msgQueue.empty()) {
		auto tmp = msgQueue.front();

		writeheader[0] = tmp.second.size();
		writeheader[1] = tmp.first;
		writebuf = tmp.second;

		// capturing buffers prevents them from releasing underlying memory
		async_write(
			*sock, boost::asio::buffer((char *)writeheader, sizeof writeheader),
			boost::asio::transfer_exactly(sizeof writeheader),
			[this](const boost::system::error_code &err, size_t n) {
				if (err) {
					std::cerr << "header write error in client \'" << nickname
							  << "\': " << err << std::endl;
					return;
				}

				async_write(
					*sock, boost::asio::buffer(writebuf),
					boost::asio::transfer_exactly(writeheader[0]),
					[this](const boost::system::error_code &err, size_t n) {
						if (err) {
							std::cerr << "content write error in client \'"
									  << nickname << "\': " << err << std::endl;
							return;
						}

						msgQueue.pop_front();
						send();
					});
			});
	}
}

void Client::asyncReceive() {
	async_read(
		*sock, boost::asio::buffer((char *)readheader, sizeof readheader),
		boost::asio::transfer_exactly(sizeof readheader),
		[this](const boost::system::error_code &err, size_t n) {
			if (err) {
				std::cerr << "header read error in client \'" << nickname
						  << "\': " << err << std::endl;
				return;
			}

			readbuf.resize(readheader[0]);
			async_read(*sock, boost::asio::buffer(readbuf),
					   boost::asio::transfer_exactly(readheader[0]),
					   [this](const boost::system::error_code &err, size_t n) {
						   if (err) {
							   std::cerr << "content read error in client \'"
										 << nickname << "\': " << err
										 << std::endl;
							   return;
						   }
						   switch (readheader[1]) {
						   case Text:
							   if (authenticated && on_read != nullptr)
								   on_read(shared_from_this());

							   break;
						   case Auth:
							   if (!authenticated) {
								   size_t ind = readbuf.find(':');
								   if (ind == std::string::npos)
									   break;

								   nickname.resize(ind);
								   password.resize(readheader[0] - ind - 1);
								   std::copy_n(readbuf.begin(), nickname.size(),
											   nickname.begin());
								   std::copy_n(readbuf.begin() + ind + 1,
											   password.size(),
											   password.begin());
								   on_auth(shared_from_this());
							   }
							   break;
						   case Room:
							   if (authenticated)
								   on_room(shared_from_this());

							   break;
						   default:
							   break;
						   };
					   });
		});
}