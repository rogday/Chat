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

	std::list<std::tuple<Event, std::string, std::function<void()>>> msgQueue;
	void threadfunc();

  public:
	std::string nickname;
	std::string password;

  public:
	Client() : socket(service){};

	void send();

	void asyncSend(Event, std::string, std::function<void()> = nullptr);
	void asyncRecieve();
	static void signalHandler(int);
	inline static Client &getInstance() { return client; };
	void shutdown() {
		socket.cancel();
		socket.close();
	};

	void start(char *, int);
	void connect_handler(const boost::system::error_code &ec);
};