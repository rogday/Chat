#pragma once

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

	uint64_t readheader[2];
	std::string readbuf;

  public:
	std::string nickname;
	std::string password;

  public:
	Client(socket_ptr sock) : authenticated(false), sock(sock) {}

	void asyncSend(Event, std::string);
	void asyncRecieve();
	void shutdown() {
		sock->cancel();
		sock->close();
	}

	inline void setAuth() { authenticated = true; }
	inline std::string &getContent() { return readbuf; }
};