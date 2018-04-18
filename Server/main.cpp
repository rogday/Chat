#include "Server.h"

int main(int argc, char *argv[]) {
	if (argc != 2) {
		std::cerr << "Wrong number of parameters!" << std::endl;
		std::cerr << "Try --help." << std::endl;
		return -1;
	}

	if (std::strcmp(argv[1], "--help") == 0) {
		std::cout << "Usage:" << std::endl;
		std::cout << "\tserver port" << std::endl;
		std::cout << "Remember: port range [0-1023] is forbidden." << std::endl;
		return 0;
	}

	int port = atoi(argv[1]);

	if (port < 1024 || port > 65535) {
		std::cerr << "Wrong port range." << std::endl;
		return -1;
	}

	// probably UB here: detached threads checking global variable [fixed]
	// TODO: async read\write, since threads get stiuck on read

	Server::startAtPort(port);

	return 0;
}