#pragma once

#include "Room.h" //not implemented yet

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <csignal>
#include <functional>
#include <iostream>
#include <list>
#include <memory>
#include <set>
#include <unordered_map>

using socket_ptr = std::shared_ptr<boost::asio::ip::tcp::socket>;

class Server {
  private:
	static Server server;

	boost::asio::io_service service;
	boost::asio::ip::tcp::acceptor acceptor;

	std::unordered_map<std::string, Room> rooms;
	std::set<std::shared_ptr<Client>> roomless;
	size_t clients;

  private:
	Server() : acceptor(service), clients(0) {
		Client::on_auth = boost::bind(&Server::onAuth, this, _1);
		Client::on_room = boost::bind(&Server::onRoom, this, _1);
	};
	Server(const Server &);
	void operator=(const Server &);

	static void signalHandler(int);

	void acceptHandler(socket_ptr, const boost::system::error_code &);
	void onAuth(std::shared_ptr<Client>);
	void onRoom(std::shared_ptr<Client>);

  public:
	inline static Server &getInstance() { return server; }
	void startAtPort(int port);
};