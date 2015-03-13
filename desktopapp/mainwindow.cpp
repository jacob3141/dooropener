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

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "settingsdialog.h"

#include <QMenu>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent, Qt::Window | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint),
    ui(new Ui::MainWindow) {
    ui->setupUi(this);

    _connected = false;

    _systemTrayIcon = new QSystemTrayIcon(this);
    _systemTrayIcon->setIcon(QIcon(":/images/appicon.png"));
    _systemTrayIcon->show();

    _systemTrayIcon->setContextMenu(new QMenu());

    QAction *showAction = _systemTrayIcon->contextMenu()->addAction(tr("S&how"));
    connect(showAction, SIGNAL(triggered()), this, SLOT(makeVisible()));

    QAction *settingsAction = _systemTrayIcon->contextMenu()->addAction(tr("&Settings"));
    connect(settingsAction, SIGNAL(triggered()), this, SLOT(showSettingsDialog()));

    QAction *quitAction = _systemTrayIcon->contextMenu()->addAction(tr("&Quit"));
    connect(quitAction, SIGNAL(triggered()), this, SLOT(quit()));

    int numberOfScreens = _desktopWidget.screenCount();
    QRect screenRect = _desktopWidget.availableGeometry(numberOfScreens - 1);

    move(screenRect.width() - width(), 0);

    _webSocket = new QWebSocket();
    _mediaPlayer = new QMediaPlayer();

    connect(_webSocket, SIGNAL(connected()), this, SLOT(connectedToServer()));
    connect(_webSocket, SIGNAL(disconnected()), this, SLOT(disconnectedFromServer()));
    connect(_webSocket, SIGNAL(textMessageReceived(QString)), this, SLOT(handleServerMessage(QString)));

    _reconnectTimer = new QTimer();
    _reconnectTimer->setInterval(1000);
    _reconnectTimer->setSingleShot(false);
    connect(_reconnectTimer, SIGNAL(timeout()), this, SLOT(tryReconnect()));
    _reconnectTimer->start();

    _hideWindowTimer = new QTimer();
    _hideWindowTimer->setInterval(1000);
    _hideWindowTimer->setSingleShot(false);
    connect(_hideWindowTimer, SIGNAL(timeout()), this, SLOT(decrementHideCounter()));
    _hideWindowTimer->start();

    _settings = new QSettings("net.cybercatalyst", "dooropener");
    ui->pushButtonMute->setChecked(_settings->value("muted").toBool());
    ui->pushButtonOpen->setEnabled(false);
    ui->pushButtonOpen->setText(tr("Connect"));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::connectedToServer() {
    ui->pushButtonOpen->setEnabled(true);
    ui->pushButtonOpen->setText(tr("Open"));
    ui->labelConnectionState->setStyleSheet("background-color: rgb(0, 250, 0);");
    _connected = true;
}

void MainWindow::disconnectedFromServer() {
    ui->labelConnectionState->setStyleSheet("background-color: rgb(200, 0, 0);");
    _connected = false;
}

void MainWindow::on_pushButtonOpen_clicked() {
    _webSocket->sendTextMessage("openDoor");
}

void MainWindow::on_pushButtonMute_clicked(bool on) {
    _settings->setValue("muted", on);
    _settings->sync();
}

void MainWindow::makeVisible() {
    ui->progressBar->setValue(10);
    show();
}

void MainWindow::decrementHideCounter() {
    if(isVisible() && _connected) {
        ui->progressBar->setValue(ui->progressBar->value() - 1);
        if(ui->progressBar->value() <= 0) {
            hide();
        }
    }
}

void MainWindow::handleServerMessage(QString message) {
    if(message.contains("doorRing")) {
        if(!_settings->value("muted").toBool()) {
            _mediaPlayer->setMedia(QUrl("qrc:///images/doorbell.m4a"));
            _mediaPlayer->setVolume(95);
            _mediaPlayer->play();
        }
        makeVisible();
    }
    if(message.contains("willOpenDoor")) {
        ui->labelConnectionState->setStyleSheet("background-color: rgb(255, 255, 255);");
        ui->pushButtonOpen->setEnabled(false);
        ui->pushButtonOpen->setText(tr("Opening"));
    }
    if(message.contains("didOpenDoor")) {
        ui->labelConnectionState->setStyleSheet("background-color: rgb(0, 250, 0);");
        ui->pushButtonOpen->setEnabled(true);
        ui->pushButtonOpen->setText(tr("Open"));
    }
}

void MainWindow::tryReconnect() {
    if(!_connected) {
        makeVisible();
        ui->pushButtonOpen->setEnabled(false);
        ui->pushButtonOpen->setText(tr("Reconnect"));
        _webSocket->close();
        _webSocket->open(QUrl("ws://" + _settings->value("server").toString()));
    }
}

void MainWindow::disconnectFromServer() {
    _connected = false;
    _reconnectTimer->start();
}

void MainWindow::showSettingsDialog() {
    SettingsDialog settingsDialog;
    settingsDialog.setIPAddress(_settings->value("server").toString());
    if(settingsDialog.exec() == QDialog::Accepted) {
        _settings->setValue("server", settingsDialog.ipAddress());
        _settings->sync();
        _connected = false;
    }
}

void MainWindow::quit() {
    close();
}
