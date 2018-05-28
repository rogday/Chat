#pragma once

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <cstdint>
#include <functional>
#include <iostream>
#include <list>
#include <memory>
#include <string>

class Client : public std::enable_shared_from_this<Client> {
  public:
	enum Event { Auth, Room, NewCommer, ClientAPI };

	std::function<void(std::shared_ptr<Client>, Event, std::string &)> on_read;
	std::function<void(std::shared_ptr<Client>)> on_error;

  private:
	static std::function<bool(std::shared_ptr<Client>)> on_auth;
	static std::function<void(std::shared_ptr<Client>, std::string)> on_room;

	std::function<void(Event, std::string)> handler;

	boost::asio::ip::tcp::socket sock;

  private:
	void send();
	void Authentication(Event, std::string);

  public:
	std::string nickname;
	std::string password;

  public:
	Client(boost::asio::ip::tcp::socket &&sock) : sock(std::move(sock)) {
		handler = boost::bind(&Client::Authentication, this, _1, _2);
	}
	~Client() { std::cout << "another one bites the dust" << std::endl; }

	void asyncSend(Event, std::string);
	void asyncReceive();
	void shutdown() {
		if (sock.is_open()) {
			sock.cancel();
			sock.close();
		}
	}
};