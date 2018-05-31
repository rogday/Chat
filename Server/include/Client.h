#pragma once

#include "Account.h"

#include <boost/asio.hpp>
#include <functional>
#include <memory>
#include <string>

class Client : public std::enable_shared_from_this<Client> {
  public:
	using Event = uint64_t;
	enum { Auth, Room, NewCommer, ClientAPI };

  private:
	static std::function<void(std::shared_ptr<Client>, Event, uint64_t,
							  std::string &)>
		on_read;
	static std::function<void(std::shared_ptr<Client>)> on_error;
	static std::function<bool(std::shared_ptr<Client>, std::string,
							  std::string)>
		on_auth;
	static std::function<bool(std::shared_ptr<Client>, uint64_t)> on_room;

	std::function<void(Event, std::string)> handler;

	boost::asio::ip::tcp::socket sock;

  private:
	void send();
	void Authentication(Event, std::string);

  public:
	std::unique_ptr<Account> account;

  public:
	Client(boost::asio::ip::tcp::socket &&);
	~Client();

	void asyncSend(Event, std::string);
	void startReceive();

	void shutdown();
};