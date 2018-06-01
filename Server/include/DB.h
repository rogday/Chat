#pragma once

#include "Account.h"
#include "Utils.h"

#include <exception>
#include <mariadb/mysql.h>
#include <memory>
#include <optional>
#include <string>

class DB {
  private:
	MYSQL *connection;

  private:
	MYSQL_RES *querry(std::string str);

  public:
	DB();
	~DB();

	std::unique_ptr<Account> getUserInfo(std::unique_ptr<API::AuthAnswer> &,
										 std::string, std::string);
	bool mayConnect(uint64_t, uint64_t);
};
