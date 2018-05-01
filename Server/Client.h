#pragma once

#include <algorithm>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <cstdint>
#include <functional>
#include <iostream>
#include <memory>
#include <string>

using socket_ptr = std::shared_ptr<boost::asio::ip::tcp::socket>;

class Client : public std::enable_shared_from_this<Client> {
  public:
	enum Event { Text, Auth, Room, NewCommer };

	std::function<void(std::shared_ptr<Client>)> on_read;
	static std::function<void(std::shared_ptr<Client>)> on_auth;
	static std::function<void(std::shared_ptr<Client>)> on_room;

  private:
	bool authenticated;

	socket_ptr sock;

	uint64_t writeheader[2];
	uint64_t written;
	std::string writebuf;

	uint64_t readheader[2];
	uint64_t read;
	std::string readbuf;

  public:
	std::string nickname;
	std::string password;

  public:
	Client(socket_ptr sock) : authenticated(false), sock(sock) {}

	void asyncSend(Event type, std::string str) {
		writeheader[0] = str.size();
		writeheader[1] = type;

		async_write(
			*sock, boost::asio::buffer((char *)writeheader, sizeof writeheader),
			boost::asio::transfer_exactly(sizeof readheader),
			[this](const boost::system::error_code &err, size_t n) {
				if (err) {
					std::cerr << "header write error in client "
							  << sock->remote_endpoint().address().to_string()
							  << std::endl;
					return;
				}

				writebuf.resize(writeheader[0]);
				async_write(
					*sock, boost::asio::buffer(writebuf),
					boost::asio::transfer_exactly(writeheader[0]),
					[this](const boost::system::error_code &err, size_t n) {
						if (err) {
							std::cerr
								<< "content write error in client "
								<< sock->remote_endpoint().address().to_string()
								<< std::endl;
							return;
						}
					});
			});
	}

	void asyncRecieve() {
		async_read(
			*sock, boost::asio::buffer((char *)readheader, sizeof readheader),
			boost::asio::transfer_exactly(sizeof readheader),
			[this](const boost::system::error_code &err, size_t n) {
				if (err) {
					std::cerr << "header read error in client "
							  << sock->remote_endpoint().address().to_string()
							  << std::endl;
					return;
				}

				readbuf.resize(readheader[0]);
				async_read(
					*sock, boost::asio::buffer(readbuf),
					boost::asio::transfer_exactly(readheader[0]),
					[this](const boost::system::error_code &err, size_t n) {
						if (err) {
							std::cerr
								<< "content read error in client "
								<< sock->remote_endpoint().address().to_string()
								<< std::endl;
						}
						switch (readheader[1]) {
						case Text:
							if (authenticated) {
								on_read(shared_from_this());
							}
							break;
						case Auth:
							if (!authenticated) {
								size_t ind = readbuf.find(':');
								nickname.resize(ind);
								password.resize(readheader[0] - ind - 1);
								std::copy_n(readbuf.begin(), nickname.size(),
											nickname.begin());
								std::copy_n(readbuf.begin() + ind + 1,
											password.size(), password.begin());
								on_auth(shared_from_this());
							}
							break;
						case Room:
							if (authenticated) {
								on_room(shared_from_this());
							}
							break;
						default:
							break;
						};
					});
			});
	}

	inline void setAuth() { authenticated = true; }
	inline std::string &getContent() { return readbuf; }
};