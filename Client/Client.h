#pragma once

#include <algorithm>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/shared_array.hpp>
#include <csignal>
#include <cstdint>
#include <functional>
#include <iostream>
#include <list>
#include <string>
#include <thread>

class Client {
  public:
	enum Event { Auth, Room, NewCommer, ClientAPI };

  private:
	static Client client;

	boost::asio::io_service service;
	boost::asio::ip::tcp::socket socket;

	uint64_t readheader[2];
	std::string readbuf;

	uint64_t writeheader[2];
	std::string writebuf;

    std::list<std::tuple<Event, std::string>> msgQueue;

  private:
	static void signalHandler(int);

	void startRecieving();
	void connect_handler(const boost::system::error_code &ec);
	void send();

  public:
	std::function<void()> login;
	std::function<void(std::string)> on_auth;
	std::function<void(std::string)> on_room;

	std::function<void(std::string)> on_read;
    std::function<void(std::string)> on_newcommer;
	std::function<void()> on_error;

	std::string nickname;
	std::string password;

  public:
	Client() : socket(service){};

	void connect(char *, int);
    void asyncSend(Event, std::string);

	inline static Client &getInstance() { return client; };

	void run() { service.run(); }
	void shutdown() {
		socket.cancel();
		socket.close();
	};
};
