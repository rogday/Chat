#include "Client.h"
#include "Server.h"
#include "Utils.h"

#include <boost/bind.hpp>
#include <cstdint>
#include <iostream>

std::function<bool(std::shared_ptr<Client>)> Client::on_auth =
	boost::bind(&Server::onAuth, &Server::getInstance(), _1);
;
std::function<void(std::shared_ptr<Client>, std::string)> Client::on_room =
	boost::bind(&Server::onRoom, &Server::getInstance(), _1, _2);

Client::Client(boost::asio::ip::tcp::socket &&sock) : sock(std::move(sock)) {
	handler = boost::bind(&Client::Authentication, this, _1, _2);
}

Client::~Client() { Utils::Info << "Client went out of scope." << std::endl; }

void Client::asyncSend(Event type, std::string str) {
	uint64_t n = str.size();
	auto buf = std::make_unique<std::string>(Utils::toStr(n) +
											 Utils::toStr(type) + str);
	auto self = shared_from_this();
	auto ptr = buf.get();

	async_write(sock, boost::asio::buffer(ptr->data(), ptr->size()),
				boost::asio::transfer_exactly(ptr->size()),
				[this, self, buf = std::move(buf)](
					const boost::system::error_code &err, size_t) {
					if (err) {
						Utils::Error << "write error in client \'" << nickname
									 << "\': " << err << std::endl;
						on_error(self);
						return;
					}
				});
}

void Client::startReceive() {
	auto header = std::make_unique<uint64_t[]>(2);
	auto self = shared_from_this();
	auto ptr = header.get();

	async_read(
		sock, boost::asio::buffer((char *)ptr, 16),
		boost::asio::transfer_exactly(16),
		[this, self, header = std::move(header)](
			const boost::system::error_code &err, size_t) mutable {
			if (err || (header[0] > (1 << 20))) {
				Utils::Error << "header read error in client \'" << nickname
							 << "\': " << err << "; size: " << header[0]
							 << std::endl;
				on_error(self);
				return;
			}

			auto buf = std::make_unique<std::string>();
			auto ptr = buf.get();
			buf->resize(header[0]);

			async_read(
				sock, boost::asio::buffer(ptr->data(), ptr->size()),
				boost::asio::transfer_exactly(ptr->size()),
				[this, self, header = std::move(header), buf = std::move(buf)](
					const boost::system::error_code &err, size_t) {
					if (err) {
						Utils::Error << "content read error in client \'"
									 << nickname << "\': " << err << std::endl;
						on_error(self);
						return;
					}

					handler((Event)header[1], *buf);
					startReceive();
				});
		});
}

void Client::Authentication(Event type, std::string str) {
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
		handler = [this](auto type, auto room) {
			if (type != Room)
				return;

			on_room(shared_from_this(), room);
			handler = [this](auto type, auto mes) {
				if (type < ClientAPI)
					return;

				on_read(shared_from_this(), type, mes);
			};
		};
}

void Client::shutdown() {
	if (sock.is_open()) {
		sock.cancel();
		sock.close();
	}
}