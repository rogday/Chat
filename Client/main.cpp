#include "Client.h"

int main(int argc, char *argv[]) {
	if (argc < 2) {
		std::cerr << "Wrong number of parameters!" << std::endl;
		std::cerr << "Try --help." << std::endl;
		return -1;
	}

	if (std::strcmp(argv[1], "--help") == 0) {
		std::cout << "Usage:" << std::endl;
		std::cout << "\t server ip \tserver port" << std::endl;
		std::cout << "Remember: port range [0-1023] is forbidden." << std::endl;
		return 0;
	}

	char *ip = argv[1];
	int port = atoi(argv[2]);

	if (port < 1024 || port > 65535) {
		std::cerr << "Wrong port range." << std::endl;
		return -1;
	}

	Client &leha = Client::getInstance();
	leha.start(ip, port);

	std::cout << "dying" << std::endl;

	return 0;
}