#pragma once

#include "Account.h"
#include "DB.h"
#include "Messages.h"
#include "Room.h"

#include <boost/asio.hpp>
#include <map>
#include <memory>
#include <unordered_set>

class Server {
	friend Client; // allow client class access callbacks

  private:
	static Server server;

	boost::asio::io_service service;
	boost::asio::ip::tcp::acceptor acceptor;
	boost::asio::ip::tcp::socket socket;
	DB database;

	std::map<API::ID, Room> rooms;
	std::unordered_set<std::shared_ptr<Client>> roomless;

  private:
	Server();
	Server(const Server &) = delete;
	void operator=(const Server &) = delete;

	void acceptHandler(const boost::system::error_code &);

	void onAuth(std::shared_ptr<Client>, std::string, std::string);
	bool onRoom(std::shared_ptr<Client>, API::ID);
	void onRead(std::shared_ptr<Client>, API::Event, API::ID, std::string);
	void onError(std::shared_ptr<Client>);

	static void signalHandler(int);

  public:
	constexpr inline static Server &getInstance() { return server; }
	void startAtPort(int port);
};