#pragma once

#include "Client.h"
#include <QMainWindow>
#include <QStandardItemModel>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
	Q_OBJECT

  public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();

  private slots:
    void on_connect_clicked();

    void on_submit_clicked();
    void on_submit_2_clicked();

	void on_send_clicked();
	void on_entry_blockCountChanged(int newBlockCount);

    void on_back_to_server_clicked();

    void on_change_user_clicked();

    void on_change_room_clicked();

public slots:
    void showConnect();
    void showLogin();
    void showRooms(std::string);
    void showChat();
    void addpeople(std::string);

    void newMes(std::string);
    void newUserlist(std::string);

  private:
	Ui::MainWindow *ui;
	Client &client = Client::getInstance();
};
