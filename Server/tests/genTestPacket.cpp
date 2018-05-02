#include <cstdint>
#include <cstring>
#include <fstream>

enum Event { Text = 0, Auth, Room, NewCommer };

uint64_t header[2];
std::fstream file("test.pkg", std::ios::binary | std::ios::out);

void writer(Event type, char *buffer) {
	header[0] = std::strlen(buffer);
	header[1] = type;

	file.write((char *)header, sizeof header);
	file.write(buffer, header[0]);
}

int main(int argc, char *argv[]) {
	writer(Auth, argv[1]);
	writer(Room, argv[2]);
	writer(Text, argv[3]);
}