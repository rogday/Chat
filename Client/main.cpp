#include "Client.h"

// copypasted
template <typename Out>
void split(const std::string &s, char delim, Out result) {
	std::stringstream ss(s);
	std::string item;
	while (std::getline(ss, item, delim)) {
		*(result++) = item;
	}
}

void inputProcess(Client &client) {
	std::string name;

	while (std::getline(std::cin, name))
		client.asyncSend(Client::Event::ClientAPI, name);
}

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

	Client &client = Client::getInstance();
	client.connect(ip, port);

	client.login = [&client]() {
		// CleanLoginScene.show()
		// .on_submit = asyncSend(Auth);

		std::string nickname, password;

		std::cout << "Connected.\nLogin: ";
		std::getline(std::cin, nickname);
		std::cout << "Password: ";
		std::getline(std::cin, password);

		client.asyncSend(Client::Event::Auth, nickname + ':' + password);
	};

	client.on_auth = [&client](std::string list) {
		// EnterRoomScene.show(rooms)
		// .on_submit = asyncSend(Room);

		std::cout << "Available rooms: " << std::endl;

		std::list<std::string> lst;
		split(list, ':', std::back_inserter(lst));

		for (auto &item : lst)
			std::cout << item << std::endl;

		std::string room;
		std::cout << "Room name: ";
		std::getline(std::cin, room);

		client.asyncSend(Client::Event::Room, room, [&client]() {
			std::thread tr(inputProcess, std::ref(client));
			tr.detach();
		});
	};

	client.on_room = [&client](std::string list) {
		// ChatScene(users)
		// .on_send = asyncSend(ClientAPI);

		std::cout << "Users online: " << std::endl;

		std::list<std::string> lst;
		split(list, ':', std::back_inserter(lst));

		for (auto &item : lst)
			std::cout << item << std::endl;
	};

	client.on_read = [&client](std::string mes) {
		// ChatScene.add(msg)
		std::cerr << mes << std::endl;
	};

	client.login();
	client.run();

	std::cout << "dying" << std::endl;

	return 0;
}