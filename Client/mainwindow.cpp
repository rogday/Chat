#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <thread>
#include <iostream>

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
    ui->stack->setFixedSize(ui->stack->currentWidget()->maximumSize());
}

void MainWindow::showLogin(){
    ui->login->clear();
    ui->password->clear();
    ui->stack->setCurrentWidget(ui->auth);
    ui->stack->setFixedSize(ui->stack->currentWidget()->maximumSize());
}

void MainWindow::showRooms(std::string buf){
    ui->stack->setCurrentWidget(ui->room);
    ui->stack->setFixedSize(ui->stack->currentWidget()->maximumSize());

    /*std::list<std::string> lst;
    split(list, ':', std::back_inserter(lst));*/

    API::AuthAnswer ans((std::string)buf);
    API::ID my_id = ans.getID();

    while (ans.isOk()){
    API::ID room_id = ans.getNextID();
    std::string room_name = ans.getNextName();
    client.room_list[room_id] = room_name;
    }

    for (auto &name : client.room_list)
        ui->rooms->addItem(QString::fromStdString(name.second));
}

void MainWindow::addpeople(std::string buf){
        API::AuthAnswer ans((std::string)buf);
        API::ID room_id = ans.getID();
        while (ans.isOk()){
           API::ID user_id = ans.getNextID();
           std::string user_name = ans.getNextName();
           client.user_list[room_id][user_id] = user_name;
        }
        showChat();
}

void MainWindow::showChat(){
   /* ui->stack->setCurrentWidget(ui->chat);
    ui->stack->setFixedSize(ui->stack->currentWidget()->maximumSize());*/

    std::set <std::string> uniname;
    for (auto &gr_id : client.user_list)
        for (auto &names : client.user_list[gr_id.first])
            uniname.emplace(names.second);

    for (auto &name : uniname)
        ui->rooms->addItem(QString::fromStdString(name));
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
    connect(&client, &Client::auth2, this, &MainWindow::addpeople);
    connect(&client, &Client::room, this, &MainWindow::showChat);

    connect(&client, &Client::read, this, &MainWindow::newMes);
    connect(&client, &Client::newcommer, this, &MainWindow::newUserlist);

    showConnect();
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::on_connect_clicked() {
    auto ip = ui->ip->toPlainText();
    auto port = ui->port->toPlainText().toInt();

    if (!ip.isEmpty() && port > 1024 && port < 65000){
        client.connectToServer(ip.toStdString(), port);
        std::thread tr(&Client::run, &client);
        tr.detach();
    }
}

void MainWindow::on_submit_clicked() {
    client.asyncSend(API::Auth,
                     ui->login->toPlainText().toStdString() + ':' +
                         ui->password->toPlainText().toStdString());
}

void MainWindow::on_submit_2_clicked() {

    ui->stack->setCurrentWidget(ui->chat);
    ui->stack->setFixedSize(ui->stack->currentWidget()->maximumSize());

    if(!ui->roomname->toPlainText().toStdString().empty())
        client.asyncSend(API::Room,
                     ui->roomname->toPlainText().toStdString());
    else{
       /* QList <QListWidgetItem*> selected=ui->rooms->selectedItems();
        if(selected.size()==1)
            client.asyncSend(API::Room,
                             selected[0]->text().toStdString());*/
        QList <QListWidgetItem*> selected=ui->rooms->selectedItems();
        for(auto& a:client.room_list)
            if(a.second==selected[0]->text().toStdString()){
                for(auto& i:client.user_list[a.first]){
                    QListWidgetItem* t = new QListWidgetItem;
                    t->setText(QString::fromStdString(i.second));
                    ui->users->addItem(t);
                }
                break;
            }
    }
}

void MainWindow::on_send_clicked() {
    QString text = ui->entry->toPlainText();
    ui->entry->clear();

    QList <QListWidgetItem*> selected=ui->rooms->selectedItems();

    if (*text.rbegin() == '\n')
        text.chop(1);

    if (text.size() != 0)
        for(auto& a:client.room_list)
            if(selected[0]->text().toStdString()==a.second){
                API::Message anmes(a.first, text.toStdString());
                client.asyncSend(API::ClientAPI,anmes.getBuf());
                break;
        }

       // client.asyncSend(API::ClientAPI, text.toStdString());
}

void MainWindow::on_entry_blockCountChanged(int newBlockCount) {
    if (newBlockCount != 1) {
        on_send_clicked();
    }
}

void MainWindow::on_back_to_server_clicked()
{
    ui->stack->setCurrentWidget(ui->server);
    ui->stack->setFixedSize(ui->stack->currentWidget()->maximumSize());
    client.shutdown();
    /*here unconnect*/
}

void MainWindow::on_change_user_clicked()
{
    ui->stack->setCurrentWidget(ui->auth);
    ui->stack->setFixedSize(ui->stack->currentWidget()->maximumSize());
    /*here unlogin*/
}

void MainWindow::on_change_room_clicked()
{
    ui->stack->setCurrentWidget(ui->room);
    ui->stack->setFixedSize(ui->stack->currentWidget()->maximumSize());
    ui->history->clear();
    ui->users->clear();
    ui->entry->clear();
    /*here another room*/
}
