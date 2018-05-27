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

  public slots:
    void showConnect();
    void showLogin();
    void showRooms(std::string);
    void showChat(std::string);

    void newMes(std::string);
    void newUserlist(std::string);

  private:
	Ui::MainWindow *ui;
	Client &client = Client::getInstance();
};
