#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <thread>

// copypasted
template <typename Out>
void split(const std::string &s, char delim, Out result) {
	std::stringstream ss(s);
	std::string item;
	while (std::getline(ss, item, delim)) {
		*(result++) = item;
	}
}

void MainWindow::showConnect(){
    ui->ip->clear();
    ui->port->clear();
    ui->stack->setCurrentWidget(ui->server);
}

void MainWindow::showLogin(){
    ui->login->clear();
    ui->password->clear();
    ui->stack->setCurrentWidget(ui->auth);
}

void MainWindow::showRooms(std::string list){
    ui->stack->setCurrentWidget(ui->room);

    std::list<std::string> lst;
    split(list, ':', std::back_inserter(lst));

    for (auto &name : lst)
        ui->rooms->addItem(QString::fromStdString(name));
}

void MainWindow::showChat(std::string list){
    ui->stack->setCurrentWidget(ui->chat);

    std::list<std::string> lst;
    split(list, ':', std::back_inserter(lst));

    for (auto &user : lst){
        QListWidgetItem* t = new QListWidgetItem;
        t->setText(QString::fromStdString(user));
        ui->users->addItem(t);
    }
}

void MainWindow::newMes(std::string mes){
    ui->history->addItem(QString::fromStdString(mes));
    ui->history->scrollToBottom();
}

void MainWindow::newUserlist(std::string nickname){
    bool found = false;
    QString nick = QString::fromStdString(nickname);

    for (int i = 0; i < ui->users->count(); ++i) {
        QListWidgetItem *item = ui->users->item(i);
        if (item->text() == nick) {
            found = true;
            ui->users->takeItem(i);
            break;
        }
    }

    if (!found) {
        ui->users->addItem(nick);
    }
}

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent), ui(new Ui::MainWindow) {
	ui->setupUi(this);

    qRegisterMetaType<std::string>("std::string");

    connect(&client, &Client::error, this, &MainWindow::showConnect);
    connect(&client, &Client::login, this, &MainWindow::showLogin);
    connect(&client, &Client::auth, this, &MainWindow::showRooms);
    connect(&client, &Client::room, this, &MainWindow::showChat);

    connect(&client, &Client::read, this, &MainWindow::newMes);
    connect(&client, &Client::newcommer, this, &MainWindow::newUserlist);

    showConnect();
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::on_connect_clicked() {
    auto ip = ui->ip->toPlainText();
    auto port = ui->port->toPlainText().toInt();

    if (!ip.isEmpty() && port >= 1000)
        client.connectToServer(ip.toStdString(), port);

	std::thread tr(&Client::run, &client);
	tr.detach();
}

void MainWindow::on_submit_clicked() {
    client.asyncSend(Client::Event::Auth,
                     ui->login->toPlainText().toStdString() + ':' +
                         ui->password->toPlainText().toStdString());
}

void MainWindow::on_submit_2_clicked() {
    client.asyncSend(Client::Event::Room,
                     ui->roomname->toPlainText().toStdString());
}

void MainWindow::on_send_clicked() {
    QString text = ui->entry->toPlainText();
    ui->entry->clear();

    if (*text.rbegin() == '\n')
        text.chop(1);

    if (text.size() != 0)
        client.asyncSend(Client::Event::ClientAPI, text.toStdString());
}

void MainWindow::on_entry_blockCountChanged(int newBlockCount) {
    if (newBlockCount != 1) {
        on_send_clicked();
    }
}
