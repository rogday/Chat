#pragma once

#include "Room.h" //not implemented yet

#include <atomic>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <csignal>
#include <functional>
#include <iostream>
#include <list>
#include <memory>
#include <mutex>
#include <thread>

using socket_ptr = std::shared_ptr<boost::asio::ip::tcp::socket>;

class Server {
  private:
	static Server server;

	boost::asio::io_service service;
	boost::asio::ip::tcp::acceptor acceptor;

	std::list<socket_ptr> roomless; // create class of Client maybe?
	std::list<std::thread> workers;
	std::list<Room> rooms;

	std::atomic<size_t> clients;
	std::mutex mutex;

  private:
	Server() : acceptor(service), clients(0){};
	Server(const Server &);
	void operator=(const Server &);

	static void signalHandler(int);

	void startAccept(socket_ptr);
	void acceptHandler(socket_ptr, const boost::system::error_code &);

	void readHandler(char *, socket_ptr, const boost::system::error_code &,
					 std::size_t);
	void writeHandler(char *, socket_ptr, const boost::system::error_code &,
					  std::size_t);

	void clientSession(socket_ptr);
	void clientOnError(socket_ptr);

  public:
	static Server &getInstance() { return server; }
	void startAtPort(int port);
};