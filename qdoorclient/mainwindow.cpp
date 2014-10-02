//
// This file is part of DoorOpener.
// Copyright (c) 2014 Jacob Dawid <jacob@omg-it.works>
//
// DoorOpener is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as
// published by the Free Software Foundation, either version 3 of the
// License, or (at your option) any later version.
//
// DoorOpener is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Affero General Public License for more details.
//
// You should have received a copy of the GNU Affero General Public
// License along with DoorOpener.
// If not, see <http://www.gnu.org/licenses/>.
//

// Own includes
#include "mainwindow.h"
#include "ui_mainwindow.h"

// Qt includes
#include <QUrl>
#include <QMediaPlayer>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    _webSocket = new QWebSocket();
    _mediaPlayer = new QMediaPlayer();
    _timer = new QTimer();
    _splashTimer = new QTimer();
    _connected = false;

    connect(_webSocket, SIGNAL(connected()), this, SLOT(connectedToServer()));
    connect(_webSocket, SIGNAL(disconnected()), this, SLOT(disconnectedFromServer()));
    connect(_webSocket, SIGNAL(textMessageReceived(QString)), this, SLOT(handleServerMessage(QString)));
    connect(_timer, SIGNAL(timeout()), this, SLOT(tryReconnect()));

    connect(_splashTimer, SIGNAL(timeout()), this, SLOT(removeSplash()));

    _timer->setInterval(1000);
    _timer->setSingleShot(false);
    _timer->start();

    _splashTimer->setInterval(1000);
    _splashTimer->setSingleShot(true);
    _splashTimer->start();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::connectedToServer()
{
    ui->labelConnectionState->setText("Connected");
    _connected = true;
}

void MainWindow::disconnectedFromServer()
{
    ui->labelConnectionState->setText("Disconnected.. Trying to reconnect.");
    _connected = false;
}

void MainWindow::on_pushButtonOpen_clicked()
{
    _webSocket->sendTextMessage("openDoor");
}

void MainWindow::removeSplash()
{
    ui->stackedWidget->setCurrentIndex(2);
}

void MainWindow::handleServerMessage(QString message) {
    if(message == "doorRing") {
        ui->labelConnectionState->setText("Ring ring..");
        if(!ui->pushButtonAuto->isChecked()) {
            _mediaPlayer->setMedia(QUrl("qrc:///images/doorbell.m4a"));
            _mediaPlayer->setVolume(95);
            _mediaPlayer->play();
        } else {
            on_pushButtonOpen_clicked();
        }
    }
    if(message.startsWith("willOpenDoor")) {
        ui->labelConnectionState->setText("Opening door.. hang on..");
    }
    if(message == "didOpenDoor") {
        ui->labelConnectionState->setText("Yo mate, it's done.");
    }
}

void MainWindow::tryReconnect() {
    if(!_connected) {
        _webSocket->close();
        _webSocket->open(QUrl("ws://localhost:1337"));
    }
}
