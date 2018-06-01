#include "Messages.h"
#include "Utils.h"

API::AuthRequest::AuthRequest(std::string &&login, std::string &&password)
	: buf(login + ":" + password), index(login.size()) {}

std::string &API::AuthRequest::getBuf() { return buf; }

API::AuthRequest::AuthRequest(std::string &&buffer)
	: buf(std::move(buffer)), index(buf.find(':')) {}

bool API::AuthRequest::isOk() { return index != std::string::npos; }

std::string API::AuthRequest::getLogin() { return buf.substr(0, index); }
std::string API::AuthRequest::getPassword() { return buf.substr(index + 1); }

API::ListPacket::ListPacket(ID id) : buf(Utils::rStr(id)), index(buf.size()) {}

void API::ListPacket::insert(ID id, std::string name) {
	buf += Utils::rStr(id) + name + ":";
}

std::string &API::ListPacket::getBuf() { return buf; }

API::ListPacket::ListPacket(std::string &&buf)
	: buf(std::move(buf)), index(0) {}

API::ID API::ListPacket::getID() { return getNextID(); }

bool API::ListPacket::isOk() { return buf.size() != index; }

API::ID API::ListPacket::getNextID() {
	ID id = Utils::rID(buf.begin() + index);
	index += sizeof(ID);

	return id;
}

std::string API::ListPacket::getNextName() {
	size_t end = buf.find(':');
	std::string s = buf.substr(index, end);
	index += end + 1;
	return s;
}

API::Message::Message(ID id, std::string buf)
	: buf(Utils::rStr(id) + std::move(buf)), index(0) {}

std::string &API::Message::getBuf() { return buf; }

API::Message::Message(std::string buf) : buf(std::move(buf)), index(0) {}

API::ID API::Message::getID() {
	ID id = Utils::rID(buf.begin());
	index += sizeof(ID);

	return id;
}

std::string API::Message::getData() { return buf.substr(index); }
