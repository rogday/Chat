#include "Client.h"
#include "Messages.h"
#include "Server.h"

#include <boost/bind.hpp>
#include <cstdint>
#include <iostream>

std::function<void(std::shared_ptr<Client>, API::Event, API::ID, std::string)>
	Client::on_read =
		boost::bind(&Server::onRead, &Server::getInstance(), _1, _2, _3, _4);

std::function<void(std::shared_ptr<Client>)> Client::on_error =
	boost::bind(&Server::onError, &Server::getInstance(), _1);

std::function<bool(std::shared_ptr<Client>, std::string, std::string)>
	Client::on_auth =
		boost::bind(&Server::onAuth, &Server::getInstance(), _1, _2, _3);
;
std::function<bool(std::shared_ptr<Client>, API::ID)> Client::on_room =
	boost::bind(&Server::onRoom, &Server::getInstance(), _1, _2);

Client::Client(boost::asio::ip::tcp::socket &&sock) : sock(std::move(sock)) {
	handler = boost::bind(&Client::Authentication, this, _1, _2);
}

Client::~Client() { Utils::Info << "Client went out of scope." << std::endl; }

void Client::asyncSend(API::Event type, std::string str) {
	API::ID n = str.size();
	auto buf =
		std::make_unique<std::string>(Utils::rStr(n) + Utils::rStr(type) + str);
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
	auto header = std::make_unique<API::ID[]>(2);
	auto self = shared_from_this();
	auto ptr = header.get();

	async_read(
		sock, boost::asio::buffer((char *)ptr, 2 * sizeof(API::ID)),
		boost::asio::transfer_exactly(2 * sizeof(API::ID)),
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

					handler((API::Event)header[1], std::move(*buf));
					startReceive();
				});
		});
}

void Client::Authentication(API::Event type, std::string str) {
	if (type != API::Auth)
		return;

	API::AuthRequest request(std::move(str));
	if (!request.isOk())
		return;

	if (on_auth(shared_from_this(), request.getLogin(), request.getPassword()))
		handler = [this](auto type, auto mes) mutable {
			// Enter in new room can be done here
			if (type < API::ClientAPI || mes.size() < sizeof(API::ID))
				return;

			API::Message msg(std::move(mes));
			API::ID room_id = msg.getID();

			auto it = account->rooms.find(room_id);
			if (it == account->rooms.end())
				return;

			on_read(shared_from_this(), type, room_id, msg.getData());
		};
}

void Client::shutdown() {
	if (sock.is_open()) {
		sock.cancel();
		sock.close();
	}
}