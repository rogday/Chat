#include "Client.h"
#include "Server.h"

#include <memory>

std::function<bool(std::shared_ptr<Client>)> Client::on_auth =
	boost::bind(&Server::onAuth, &Server::getInstance(), _1);
;
std::function<void(std::shared_ptr<Client>, std::string)> Client::on_room =
	boost::bind(&Server::onRoom, &Server::getInstance(), _1, _2);

void Client::asyncSend(Event type, std::string str) {
	auto header = std::make_unique<uint64_t[]>(2);
	auto buf = std::make_unique<std::string>(str);
	auto ptr = header.get();

	header[0] = str.size();
	header[1] = type;

	auto self = shared_from_this();

	async_write(sock, boost::asio::buffer((char *)ptr, 16),
				boost::asio::transfer_exactly(16),
				[this, self, header = std::move(header), buf = std::move(buf)](
					const boost::system::error_code &err, size_t) mutable {
					if (err) {
						std::cerr << "header write error in client \'"
								  << nickname << "\': " << err << std::endl;

						on_error(self);
						return;
					}

					auto ptr = buf.get();
					async_write(
						sock, boost::asio::buffer(ptr->data(), ptr->size()),
						boost::asio::transfer_exactly(ptr->size()),
						[this, self, buf = std::move(buf)](
							const boost::system::error_code &err, size_t) {
							if (err) {
								std::cerr << "content write error in client \'"
										  << nickname << "\': " << err
										  << std::endl;
								on_error(self);
								return;
							}
						});
				});
}

void Client::asyncReceive() {
	auto header = std::make_unique<uint64_t[]>(2);
	auto self = shared_from_this();
	auto ptr = header.get();

	async_read(
		sock, boost::asio::buffer((char *)ptr, 16),
		boost::asio::transfer_exactly(16),
		[this, self, header = std::move(header)](
			const boost::system::error_code &err, size_t) mutable {
			if (err || (header[0] > (1 << 20))) {
				std::cerr << "header read error in client \'" << nickname
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
						std::cerr << "content read error in client \'"
								  << nickname << "\': " << err << std::endl;
						on_error(self);
						return;
					}

					handler((Event)header[1], *buf);
					asyncReceive();
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