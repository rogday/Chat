#include "DB.h"

MYSQL_RES *DB::querry(std::string str) {
	if (mysql_query(connection, str.data()))
		return nullptr;

	return mysql_store_result(connection);
}

DB::DB() : connection(mysql_init(nullptr)) {
	if (!connection ||
		!mysql_real_connect(connection, "localhost", "root", "duck",
							"messenger", 3306, nullptr, 0))
		throw std::runtime_error(mysql_error(connection));
}

DB::~DB() { mysql_close(connection); }

std::unique_ptr<Account> DB::getUserInfo(std::string &s, std::string login,
										 std::string password) {
	std::string q = "select id from users where login = '" + login +
					"' and password = '" + password + "'";

	MYSQL_RES *result = querry(q);
	if (!result)
		return nullptr;

	MYSQL_ROW row = mysql_fetch_row(result);

	if (!row)
		return nullptr;

	uint64_t id = Utils::toU64(row[0]);
	mysql_free_result(result);

	q = "select id,name from map,rooms where user_id='" + std::to_string(id) +
		"' and room_id = id";

	result = querry(q);
	if (!result)
		return nullptr;

	auto account = std::make_unique<Account>();
	account->login = login;
	account->password = password;
	account->id = id;

	s = Utils::toStr(id);
	while ((row = mysql_fetch_row(result))) {
		account->rooms.insert(Utils::toU64(row[0]));
		s += Utils::toStr(Utils::toU64(row[0])) + row[1] + ":";
	}

	mysql_free_result(result);
	return account;
}

bool DB::mayConnect(uint64_t user_id, uint64_t room_id) {
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