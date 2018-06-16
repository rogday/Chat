#pragma once

#include "Account.h"
#include "Utils.h"

#include <SQLiteCpp/SQLiteCpp.h>
#include <boost/asio.hpp>
#include <functional>
#include <string>

class DB {
  private:
	SQLite::Database db;
	boost::asio::io_service &service;

  public:
	DB(boost::asio::io_service &);
	~DB() = default;

	void getUserInfo(std::string, std::string,
					 std::function<void(API::AuthAnswer, Account)>,
					 std::function<void()>);
	// bool mayConnect(API::ID, API::ID);
};
