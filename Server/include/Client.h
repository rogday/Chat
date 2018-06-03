#pragma once

#include "Account.h"
#include "Messages.h"

#include <boost/asio.hpp>
#include <functional>
#include <memory>
#include <string>

class Client : public std::enable_shared_from_this<Client> {
  public:
	std::unique_ptr<Account> account;

  private:
	static std::function<void(std::shared_ptr<Client>, API::Event, API::ID,
							  std::string)>
		on_read;
	static std::function<void(std::shared_ptr<Client>)> on_error;
	static std::function<bool(std::shared_ptr<Client>, std::string,
							  std::string)>
		on_auth;
	static std::function<bool(std::shared_ptr<Client>, API::ID)> on_room;

	std::function<void(API::Event, std::string)> handler;
	boost::asio::ip::tcp::socket sock;

  private:
	void Authentication(API::Event, std::string);

  public:
	Client(boost::asio::ip::tcp::socket &&);
	~Client();

	void asyncSend(API::Event, std::string = "");
	void startReceive();

	void shutdown();
};