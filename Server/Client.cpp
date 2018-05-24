#include "Client.h"
#include "Server.h"

std::function<bool(std::shared_ptr<Client>)> Client::on_auth =
	std::bind(&Server::onAuth, &Server::getInstance(), std::placeholders::_1);
;
std::function<void(std::shared_ptr<Client>)> Client::on_room =
	std::bind(&Server::onRoom, &Server::getInstance(), std::placeholders::_1);

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

		auto self = shared_from_this();

		async_write(
			*sock, boost::asio::buffer((char *)writeheader, sizeof writeheader),
			boost::asio::transfer_exactly(sizeof writeheader),
			[this, self](const boost::system::error_code &err, size_t) {
				if (err) {
					std::cerr << "header write error in client \'" << nickname
							  << "\': " << err << std::endl;

					on_error(self);
					return;
				}

				async_write(
					*sock, boost::asio::buffer(writebuf),
					boost::asio::transfer_exactly(writeheader[0]),
					[this, self](const boost::system::error_code &err, size_t) {
						if (err) {
							std::cerr << "content write error in client \'"
									  << nickname << "\': " << err << std::endl;
							on_error(self);
							return;
						}

						msgQueue.pop_front();
						send();
					});
			});
	}
}

void Client::asyncReceive() {
	auto self = shared_from_this();

	async_read(
		*sock, boost::asio::buffer((char *)readheader, sizeof readheader),
		boost::asio::transfer_exactly(sizeof readheader),
		[this, self](const boost::system::error_code &err, size_t) {
			if (err) {
				std::cerr << "header read error in client \'" << nickname
						  << "\': " << err << std::endl;
				on_error(self);
				return;
			}

			readbuf.resize(readheader[0]);
			async_read(
				*sock, boost::asio::buffer(readbuf),
				boost::asio::transfer_exactly(readheader[0]),
				[this, self](const boost::system::error_code &err, size_t) {
					if (err) {
						std::cerr << "content read error in client \'"
								  << nickname << "\': " << err << std::endl;
						on_error(self);
						return;
					}

					handler((Event)readheader[1], readbuf);
					asyncReceive();
				});
		});
}

void Client::Authentication(Event type, std::string &str) {
	if (type != Auth)
		return;

	size_t ind = str.find(':');

	if (ind == std::string::npos)
		return;

	nickname.resize(ind);
	password.resize(str.size() - ind - 1);

	std::copy_n(str.begin(), nickname.size(), nickname.begin());
	std::copy_n(str.begin() + ind + 1, password.size(), password.begin());

	if (on_auth(shared_from_this()))
		handler = [this](auto type, auto &) {
			if (type != Room)
				return;

			on_room(shared_from_this());
			handler = [this](auto type, auto &mes) {
				if (type < ClientAPI)
					return;

				on_read(shared_from_this(), mes);
			};
		};
}