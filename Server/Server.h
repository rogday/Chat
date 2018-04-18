#include <atomic>
#include <boost/asio.hpp>
#include <csignal>
#include <exception>
#include <functional>
#include <iostream>
#include <list>
#include <memory>
#include <string>
//#include <thread>

using socket_ptr = std::shared_ptr<boost::asio::ip::tcp::socket>;

class Server {
  private:
	static Server server;
	boost::asio::io_service service;
	boost::asio::ip::tcp::acceptor acceptor;
	std::atomic<bool> exitServer;
	std::list<socket_ptr> list;

  private:
	Server() : acceptor(service){};
	Server(const Server &);
	void operator=(const Server &);

	static void signalHandler(int);
	void startAccept(socket_ptr);
	void acceptHandler(socket_ptr, const boost::system::error_code &);
	void readHandler(char *, socket_ptr, const boost::system::error_code &,
					 std::size_t);
	void writeHandler(char *, socket_ptr, const boost::system::error_code &,
					  std::size_t);
	void clientSession(socket_ptr, size_t);

  public:
	static Server &getInstance() { return server; }
	void startAtPort(int port);
};