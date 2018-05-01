#include "Client.h"
#include "Room.h"
#include "Server.h"

std::function<void(std::shared_ptr<Client>)> Client::on_auth = nullptr;
std::function<void(std::shared_ptr<Client>)> Client::on_room = nullptr;