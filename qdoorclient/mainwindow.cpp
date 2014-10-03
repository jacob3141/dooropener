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
#include <QDateTime>

const int splashScreenIndex     = 0;
const int mainScreenIndex       = 1;
const int settingsScreenIndex   = 2;

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
    _dontUpdateVideoFrame = false;

    connect(_webSocket, SIGNAL(connected()), this, SLOT(connectedToServer()));
    connect(_webSocket, SIGNAL(disconnected()), this, SLOT(disconnectedFromServer()));
    connect(_webSocket, SIGNAL(textMessageReceived(QString)), this, SLOT(handleServerMessage(QString)));
    connect(_timer, SIGNAL(timeout()), this, SLOT(tryReconnect()));

    connect(_splashTimer, SIGNAL(timeout()), this, SLOT(removeSplash()));

    connect(ui->stackedWidget, SIGNAL(animationFinished()), this, SLOT(slideFinished()));

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

void MainWindow::slideFinished()
{
    _dontUpdateVideoFrame = false;
}

void MainWindow::connectedToServer()
{
    ui->labelConnectionState->setText("Connected");
    ui->labelConnectionState->setStyleSheet("background-color: rgb(0, 100, 0); color: white; font-size: 32px;");
    _connected = true;
}

void MainWindow::disconnectedFromServer()
{
    ui->labelConnectionState->setText("Disconnected.. Trying to reconnect.");
    ui->labelConnectionState->setStyleSheet("background-color: rgb(70, 70, 0); color: white; font-size: 32px;");
    _connected = false;
}

void MainWindow::on_openPushButton_clicked()
{
    _webSocket->sendTextMessage("openDoor");
}

void MainWindow::on_settingsPushButton_clicked()
{
    _dontUpdateVideoFrame = true;
    ui->stackedWidget->slideInAtIndex(settingsScreenIndex, SlidingStackedWidget::RightToLeft);
}

void MainWindow::on_serverIPLineEdit_returnPressed()
{
    on_saveSettingsPushButton_clicked();
}

void MainWindow::on_settingsBackPushButton_clicked()
{
    _dontUpdateVideoFrame = true;
    ui->stackedWidget->slideInAtIndex(mainScreenIndex, SlidingStackedWidget::LeftToRight);
}

void MainWindow::on_saveSettingsPushButton_clicked()
{
    _dontUpdateVideoFrame = true;
    ui->stackedWidget->slideInAtIndex(mainScreenIndex, SlidingStackedWidget::LeftToRight);
}

void MainWindow::removeSplash()
{
    _dontUpdateVideoFrame = true;
    ui->stackedWidget->slideInAtIndex(mainScreenIndex, SlidingStackedWidget::RightToLeft);
}

void MainWindow::handleServerMessage(QString message) {
    if(message.contains("doorRing")) {
        ui->labelConnectionState->setText("Ring ring..");
        if(!ui->autoPushButton->isChecked()) {
            _mediaPlayer->setMedia(QUrl("qrc:///images/doorbell.m4a"));
            _mediaPlayer->setVolume(95);
            _mediaPlayer->play();
        } else {
            // Simulate pushing the open button
            on_openPushButton_clicked();
        }
    }
    if(message.contains("willOpenDoor")) {
        ui->labelConnectionState->setText("Opening door.. hang on..");
    }
    if(message.contains("didOpenDoor")) {
        ui->labelConnectionState->setText("Yo mate, it's done.");
    }
    if(message.startsWith("image") && !_dontUpdateVideoFrame) {
        // Remove "image ", ie. first six characters to obtain base64 encoded data
        message.remove(0, 6);

        QBuffer buffer;
        buffer.setData(QByteArray::fromBase64(message.toUtf8()));
        if(!_monitorImage.loadFromData(buffer.buffer(), "PNG")) {
            qDebug() << "Failed to load image from data";
        } else {
            ui->imageLabel->setPixmap(QPixmap::fromImage(_monitorImage));
            ui->imageInfoLabel->setText(QString("Taken on %1").arg(QDateTime::currentDateTime().toString()));
        }
    }
}

void MainWindow::tryReconnect() {
    if(!_connected) {
        _webSocket->close();
        _webSocket->open(QUrl("ws://192.168.0.108:1337"));
    }
}
