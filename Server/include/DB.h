#pragma once

#include "Account.h"
#include "Utils.h"

#include <boost/asio.hpp>
#include <functional>
#include <mariadb/mysql.h>
#include <string>

class DB {
  private:
	MYSQL *connection;
	boost::asio::io_service &service;

  private:
	MYSQL_RES *querry(std::string str);

  public:
	DB(boost::asio::io_service &);
	~DB();

	void getUserInfo(std::string, std::string,
					 std::function<void(API::AuthAnswer, Account)>,
					 std::function<void()>);
	bool mayConnect(API::ID, API::ID);
};
