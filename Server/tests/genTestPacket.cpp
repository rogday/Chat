#include <cstdint>
#include <cstring>
#include <iostream>

enum Event { Text = 0, Auth, Room, NewCommer };

uint64_t header[2];

void writer(Event type, char *buffer) {
	header[0] = std::strlen(buffer);
	header[1] = type;

	std::cout.write((char *)header, sizeof header);
	std::cout.write(buffer, header[0]);
}

int main(int argc, char *argv[]) {
	writer(Auth, argv[1]);
	writer(Room, argv[2]);
	writer(Text, argv[3]);
}