#include "Client.h"
#include <cstdint>

using namespace boost::asio;

Client Client::client;

void Client::connectToServer(std::string ip, int port) {
	signal(SIGINT, signalHandler);

	ip::tcp::endpoint ep(ip::address::from_string(ip), port);

	socket.async_connect(
		ep, std::bind(&Client::connect_handler, this, std::placeholders::_1));
}

void Client::connect_handler(const boost::system::error_code &ec) {
	if (ec) {
		std::cout << "connect error: " << ec << std::endl;
        emit error();
	} else {
        emit login();
		startRecieving();
	}
}

void Client::signalHandler(int n) {
	client.shutdown();

	signal(n, signalHandler);
}

void Client::asyncSend(API::Event type, std::string str) {
    API::ID n = str.size();
    auto buf = std::make_unique<std::string>(std::string(reinterpret_cast<char *>(&n), sizeof n) +
                                        std::string(reinterpret_cast<char *>(&type), sizeof type) + str);
    auto ptr = buf.get();

    async_write(
        socket,
        boost::asio::buffer(ptr->data(), ptr->size()),
        boost::asio::transfer_exactly(ptr->size()),
        [this, buf = std::move(buf)](const boost::system::error_code &err, size_t) {
            if (err) {
                std::cerr << "header write error: " << err << std::endl;
                emit error();
                return;
            }
        });
}

void Client::startRecieving() {
    auto header =std::make_unique<API::ID[]>(2);
    auto ptr = header.get();

	async_read(socket,
               boost::asio::buffer((char *)ptr, 16),
               boost::asio::transfer_exactly(16),
               [this, header = std::move(header)](const boost::system::error_code &err, size_t) mutable {
                   if (err || (header[0] > (1 << 20))) {
					   std::cerr << "header read error: " << err << std::endl;
                       emit error();
					   return;
				   }

                   auto buf = std::make_unique<std::string>();
                   auto ptr = buf.get();
                   buf->resize(header[0]);

                   async_read(socket,
                   boost::asio::buffer((void*)ptr->data(), ptr->size()),
                   boost::asio::transfer_exactly(ptr->size()),
                         [this, header = std::move(header), buf = std::move(buf)]
                              (const boost::system::error_code &err, size_t) {
								  if (err) {
									  std::cerr << "content read error: " << err
												<< std::endl;
                                      emit error();
									  return;
								  }

                                  if ((API::Event)header[1] >= API::ClientAPI) {
                                      emit read(*buf);
									  // there you can be sure that it's message
									  // from another client and add your own
									  // API for files, music, voice, etc
								  } else
                                      switch ((API::Event)header[1]) {
                                      case API::Auth:
                                          if ((*buf).size()==0)
                                              emit login();
                                          else
                                                emit auth(buf->substr(0));//T
										  break;
                                      case API::Room:
                                          emit auth2(*buf);
										  break;
                                      case API::NewCommer:
                                          emit newcommer(*buf);
										  break;
									  default:
										  std::cerr << "Unimplemented feature: "
                                                    << header[1] << ' '
                                                    << *buf << std::endl;
                                      }

								  startRecieving();
							  });
               });
}
