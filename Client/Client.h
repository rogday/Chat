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
#include <QObject>

class Client: public QObject {
    Q_OBJECT

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

  signals:
    void login();

    void auth(std::string);
    void room(std::string);

    void read(std::string);
    void newcommer(std::string);
    void error();

  public:

	std::string nickname;
	std::string password;

  public:
	Client() : socket(service){};

	void connectToServer(std::string, int);
	void asyncSend(Event, std::string);

	inline static Client &getInstance() { return client; };

	void run() { service.run(); }
	void shutdown() {
		socket.cancel();
		socket.close();
	};
};
