#pragma once

#include "Room.h"

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <csignal>
#include <functional>
#include <iostream>
#include <memory>
#include <set>
#include <unordered_map>

class Server {
	friend Client; // allow client class access callbacks

  private:
	static Server server;

	boost::asio::io_service service;
	boost::asio::ip::tcp::acceptor acceptor;
	boost::asio::ip::tcp::socket socket;

	std::unordered_map<std::string, Room> rooms;
	std::set<std::shared_ptr<Client>> roomless;

  private:
	Server();
	Server(const Server &) = delete;
	void operator=(const Server &) = delete;

	void acceptHandler(const boost::system::error_code &);

	bool onAuth(std::shared_ptr<Client>);
	void onRoom(std::shared_ptr<Client>, std::string);
	void onError(std::shared_ptr<Client>);

	static void signalHandler(int);

  public:
	constexpr inline static Server &getInstance() { return server; }
	void startAtPort(int port);
};