#pragma once

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <cstdint>
#include <functional>
#include <iostream>
#include <list>
#include <memory>
#include <string>

using socket_ptr = std::shared_ptr<boost::asio::ip::tcp::socket>;

class Client : public std::enable_shared_from_this<Client> {
  public:
	enum Event { Auth, Room, NewCommer, ClientAPI };

	std::function<void(std::shared_ptr<Client>, std::string &)> on_read;
	std::function<void(std::shared_ptr<Client>)> on_error;

  private:
	static std::function<bool(std::shared_ptr<Client>)> on_auth;
	static std::function<void(std::shared_ptr<Client>)> on_room;
	std::function<void(Event, std::string &)> handler;

	socket_ptr sock;

	uint64_t readheader[2];
	std::string readbuf;

	uint64_t writeheader[2];
	std::string writebuf;

	std::list<std::pair<Event, std::string>> msgQueue;

  private:
	void send();
	void Authentication(Event, std::string &);

  public:
	std::string nickname;
	std::string password;

  public:
	Client(socket_ptr sock) : sock(sock) {
		handler = boost::bind(&Client::Authentication, this, _1, _2);
	}

	void asyncSend(Event, std::string);
	void asyncReceive();
	void shutdown() {
		if (sock->is_open()) {
			sock->cancel();
			sock->close();
		}
	}

	inline std::string &getContent() { return readbuf; }
	inline Event getType() { return (Event)readheader[1]; }
};