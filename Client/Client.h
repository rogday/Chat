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
#include <map>
#include <set>

#include <Messages.h>
#include <string>
#include <stack>

class Client: public QObject {
    Q_OBJECT
  private:
	static Client client;

	boost::asio::io_service service;
	boost::asio::ip::tcp::socket socket;

  private:
	static void signalHandler(int);

	void startRecieving();
	void connect_handler(const boost::system::error_code &ec);

  signals:
    void login();

    void auth(std::string);
    void auth2(std::string);
    void room(std::string);

    void read(std::string);
    void newcommer(std::string);
    void error();

  public:
	std::string nickname;
	std::string password;

  public:
    std::map<API::ID,std::string> room_list;
    std::map<API::ID,std::stack<std::string>> room_history;
    std::map<API::ID,std::map<API::ID,std::string>> user_list;//room id + [user name + use id]

	Client() : socket(service){};

	void connectToServer(std::string, int);
    void asyncSend(API::Event, std::string);

	inline static Client &getInstance() { return client; };

	void run() { service.run(); }
	void shutdown() {
		socket.cancel();
		socket.close();
	};
};
