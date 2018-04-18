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
	// static Server server;
	static boost::asio::io_service service;
	static boost::asio::ip::tcp::acceptor acceptor;
	static std::atomic<bool> exitServer;
	static std::list<socket_ptr> list;

  private:
	Server(){};

	static void signalHandler(int);
	static void startAccept(socket_ptr);
	static void acceptHandler(socket_ptr, const boost::system::error_code &);
	static void readHandler(char *, socket_ptr,
							const boost::system::error_code &, std::size_t);
	static void writeHandler(char *, socket_ptr,
							 const boost::system::error_code &, std::size_t);
	static void clientSession(socket_ptr, size_t);

  public:
	static void startAtPort(int port);
};