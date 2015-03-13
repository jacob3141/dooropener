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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSystemTrayIcon>
#include <QDesktopWidget>
#include <QWebSocket>
#include <QMediaPlayer>
#include <QTimer>
#include <QSettings>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void connectedToServer();
    void disconnectedFromServer();
    void handleServerMessage(QString message);
    void tryReconnect();

    void showSettingsDialog();
    void quit();

    void on_pushButtonOpen_clicked();
    void on_pushButtonMute_clicked(bool on);

    void makeVisible();
    void decrementHideCounter();

private:
    void disconnectFromServer();
    Ui::MainWindow *ui;

    QSystemTrayIcon *_systemTrayIcon;
    QDesktopWidget _desktopWidget;

    QWebSocket *_webSocket;
    QMediaPlayer *_mediaPlayer;
    QTimer *_reconnectTimer;

    QTimer *_hideWindowTimer;

    QSettings *_settings;
    bool _connected;
};

#endif // MAINWINDOW_H
