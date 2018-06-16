#include "DB.h"

DB::DB(boost::asio::io_service &service)
	: db("../database.db", SQLite::OPEN_READWRITE), service(service) {}

void DB::getUserInfo(std::string login, std::string password,
					 std::function<void(API::AuthAnswer, Account)> success,
					 std::function<void()> error) {
	try {
		SQLite::Statement q1(db, "SELECT id FROM users WHERE login = '" +
									 login + "' AND password = '" + password +
									 "'");

		if (!q1.executeStep()) {
			service.post(error);
			return;
		}

		API::ID id = q1.getColumn(0).getInt64();

		Account account;
		account.login = login;
		account.password = password;
		account.id = id;

		std::cout << "userID: " << id << std::endl;

		SQLite::Statement q2(db,
							 "SELECT id,name FROM map,rooms WHERE user_id='" +
								 std::to_string(id) + "' AND room_id = id");

		API::AuthAnswer auth(id);
		while (q2.executeStep()) {
			API::ID room_id = q2.getColumn(0).getInt64();
			std::string room_name = q2.getColumn(1).getText();

			account.rooms.insert(room_id);
			auth.insert(room_id, room_name);
		}

		service.post(std::bind(success, auth, account));
	} catch (std::exception &e) {
		std::cerr << e.what() << std::endl;
	}
}

/*bool DB::mayConnect(API::ID user_id, API::ID room_id) {
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
};*/