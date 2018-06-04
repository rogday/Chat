#include "DB.h"

#include <exception>

MYSQL_RES *DB::querry(std::string str) {
	if (mysql_query(connection, str.data()))
		return nullptr;

	return mysql_store_result(connection);
}

DB::DB(boost::asio::io_service &service)
	: connection(mysql_init(nullptr)), service(service) {
	if (!connection ||
		!mysql_real_connect(connection, "localhost", "root", "duck",
							"messenger", 3306, nullptr, 0))
		throw std::runtime_error(mysql_error(connection));
}

DB::~DB() { mysql_close(connection); }

void DB::getUserInfo(std::string login, std::string password,
					 std::function<void(API::AuthAnswer, Account)> success,
					 std::function<void()> error) {
	std::string q = "select id from users where login = '" + login +
					"' and password = '" + password + "'";

	MYSQL_RES *result = querry(q);
	if (!result) {
		service.post(error);
		return;
	}

	MYSQL_ROW row = mysql_fetch_row(result);

	if (!row) {
		service.post(error);
		return;
	}

	API::ID id = Utils::toID(row[0]);
	mysql_free_result(result);

	q = "select id,name from map,rooms where user_id='" + std::to_string(id) +
		"' and room_id = id";

	result = querry(q);
	if (!result) {
		service.post(error);
		return;
	}

	Account account;
	account.login = login;
	account.password = password;
	account.id = id;

	API::AuthAnswer auth(id);
	while ((row = mysql_fetch_row(result))) {
		account.rooms.insert(Utils::toID(row[0]));
		auth.insert(Utils::toID(row[0]), row[1]);
	}

	mysql_free_result(result);
	service.post(std::bind(success, auth, account));
}

bool DB::mayConnect(API::ID user_id, API::ID room_id) {
	std::string q = "select scope from map,rooms where user_id = '" +
					std::to_string(user_id) + "' and room_id = '" +
					std::to_string(room_id) + "' and room_id=id";
	// add table of allowed users for private rooms

	MYSQL_RES *result = querry(q);
	if (!result)
		return false;

	MYSQL_ROW row = mysql_fetch_row(result);

	if (!row)
		return false;

	bool ans = false;

	if (row[0][0] == '1')
		ans = true;

	mysql_free_result(result);
	return ans;
};