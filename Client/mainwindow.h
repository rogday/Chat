#pragma once

#include <QMainWindow>
#include <QStandardItemModel>
#include "Client.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_send_clicked();
    void on_entry_blockCountChanged(int newBlockCount);
    void on_submit_clicked();
    void on_submit_2_clicked();

private:
    Ui::MainWindow *ui;
    Client& client = Client::getInstance();
};
