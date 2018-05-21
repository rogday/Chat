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

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent), ui(new Ui::MainWindow) {
	ui->setupUi(this);

	client.connect("127.0.0.1", 6666);

	client.login = [this]() {
		ui->login->clear();
		ui->password->clear();

		ui->stack->setCurrentWidget(ui->auth);
	};

	client.on_auth = [this](std::string list) {
		ui->stack->setCurrentWidget(ui->room);

		std::list<std::string> lst;
		split(list, ':', std::back_inserter(lst));

		for (auto &name : lst)
			ui->rooms->addItem(QString::fromStdString(name));
	};

	client.on_room = [this](std::string list) {
		ui->stack->setCurrentWidget(ui->chat);

		std::list<std::string> lst;
		split(list, ':', std::back_inserter(lst));

		for (auto &user : lst)
			ui->users->addItem(QString::fromStdString(user));
	};

	client.on_read = [this](std::string mes) {
		ui->history->addItem(QString::fromStdString(mes));
	};

	client.on_newcommer = [this](std::string nickname) {
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
	};

	client.login();
	std::thread tr(&Client::run, &client);
	tr.detach();
}

MainWindow::~MainWindow() { delete ui; }

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

void MainWindow::on_submit_clicked() {
	client.asyncSend(Client::Event::Auth,
					 ui->login->toPlainText().toStdString() + ':' +
						 ui->password->toPlainText().toStdString());
}

void MainWindow::on_submit_2_clicked() {
	client.asyncSend(Client::Event::Room,
					 ui->roomname->toPlainText().toStdString());
}
