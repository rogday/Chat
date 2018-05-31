#include "Client.h"
#include "Server.h"
#include "Utils.h"

#include <boost/bind.hpp>
#include <cstdint>
#include <iostream>

std::function<void(std::shared_ptr<Client>, Client::Event, uint64_t,
				   std::string &)>
	Client::on_read =
		boost::bind(&Server::onRead, &Server::getInstance(), _1, _2, _3, _4);

std::function<void(std::shared_ptr<Client>)> Client::on_error =
	boost::bind(&Server::onError, &Server::getInstance(), _1);

std::function<bool(std::shared_ptr<Client>, std::string, std::string)>
	Client::on_auth =
		boost::bind(&Server::onAuth, &Server::getInstance(), _1, _2, _3);
;
std::function<bool(std::shared_ptr<Client>, uint64_t)> Client::on_room =
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
						Utils::Error << "Write error in client \'"
									 << account.get() << "\': " << err
									 << std::endl;
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
				Utils::Error << "Header read error in client \'"
							 << account.get() << "\': " << err
							 << "; size: " << header[0] << std::endl;
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
						Utils::Error << "Content read error in client \'"
									 << account.get() << "\': " << err
									 << std::endl;
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

	std::string login = str.substr(0, ind), password = str.substr(ind + 1);

	if (on_auth(shared_from_this(), login, password))
		handler = [this](auto type, auto mes) mutable {
			// Enter in new room can be done here
			if (type < ClientAPI || mes.size() < sizeof(uint64_t))
				return;

			uint64_t room_id = 0;
			std::copy_n(mes.begin(), sizeof room_id, &room_id);
			mes = mes.substr(sizeof room_id);

			auto it = account->rooms.find(room_id);
			if (it == account->rooms.end())
				return;

			on_read(shared_from_this(), type, room_id, mes);
		};
}

void Client::shutdown() {
	if (sock.is_open()) {
		sock.cancel();
		sock.close();
	}
}