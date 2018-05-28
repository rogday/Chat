#pragma once

#include <boost/asio.hpp>
#include <functional>
#include <memory>
#include <string>

class Client : public std::enable_shared_from_this<Client> {
  public:
	using Event = uint64_t;
	enum { Auth, Room, NewCommer, ClientAPI };

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
	Client(boost::asio::ip::tcp::socket &&);
	~Client();

	void asyncSend(Event, std::string);
	void startReceive();

	void shutdown();
};