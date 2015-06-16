//
// This file is part of DoorOpener.
// Copyright (c) 2014-2015 Jacob Dawid <jacob@omg-it.works>
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

// Qt includes
#include <QDebug>
#include <QFileInfo>
#include <QDir>
#include <QWebSocket>
#include <QJsonDocument>
#include <QJsonObject>
#include <QBuffer>
#include <QFile>

// Own includes
#include "websocketserver.h"
#include "doordriver.h"

WebSocketServer::WebSocketServer(QObject *parent)
    : QWebSocketServer("WebSocketServer",
                       QWebSocketServer::NonSecureMode,
                       parent) {
    _connectedClientsSemaphore = new QSemaphore(1);

    // Internal signal for handling new client connections.
    connect(this, SIGNAL(newConnection()),
            this, SLOT(handlePendingConnections()));

    // Door driver signals
    connect(&DoorDriver::instance(), SIGNAL(opened()),
            this, SLOT(broadcastDoorOpened()));
    connect(&DoorDriver::instance(), SIGNAL(closed()),
            this, SLOT(broadcastDoorClosed()));
    connect(&DoorDriver::instance(), SIGNAL(ring()),
            this, SLOT(broadcastRing()));
}

void WebSocketServer::handlePendingConnections() {
    while(hasPendingConnections()) {
        QWebSocket *clientSocket = nextPendingConnection();

        _connectedClientsSemaphore->acquire();
        connect(clientSocket, SIGNAL(disconnected()),
                this, SLOT(clientDisconnected()));
        connect(clientSocket, SIGNAL(textMessageReceived(QString)),
                this, SLOT(clientMessageReceived(QString)));
        connect(clientSocket, SIGNAL(error(QAbstractSocket::SocketError)),
                this, SLOT(clientErrorOccurred(QAbstractSocket::SocketError)));
        _connectedClients.append(clientSocket);
        _connectedClientsSemaphore->release();
    }
}

void WebSocketServer::clientDisconnected() {
    QWebSocket* clientSocket = dynamic_cast<QWebSocket*>(sender());
    if(!clientSocket) {
        return;
    }
    _connectedClientsSemaphore->acquire();
    _connectedClients.removeAll(clientSocket);
    _connectedClientsSemaphore->release();
    clientSocket->deleteLater();
}

void WebSocketServer::clientMessageReceived(QString message) {
    QWebSocket* clientSocket = dynamic_cast<QWebSocket*>(sender());
    if(!clientSocket) {
        return;
    }

    if(message.contains("openDoor")) {
        sendOpenCommand();
    }
}

void WebSocketServer::clientErrorOccurred(QAbstractSocket::SocketError error) {
    Q_UNUSED(error);
    QWebSocket* clientSocket = dynamic_cast<QWebSocket*>(sender());
    if(!clientSocket) {
        return;
    }
    _connectedClientsSemaphore->acquire();
    _connectedClients.removeAll(clientSocket);
    _connectedClientsSemaphore->release();

    clientSocket->close();
    clientSocket->deleteLater();
}

void WebSocketServer::broadcast(QString message) {
    _connectedClientsSemaphore->acquire();
    foreach(QWebSocket* webSocket, _connectedClients) {
        webSocket->sendTextMessage(message);
    }
    _connectedClientsSemaphore->release();
}

void WebSocketServer::sendOpenCommand() {
    broadcast("willOpenDoorCommand");
    DoorDriver::instance().open();
}

void WebSocketServer::broadcastDoorOpened() {
    broadcast("didOpenDoorCommand");
}

void WebSocketServer::broadcastDoorClosed() {
    broadcast("didCloseDoorCommand");
}

void WebSocketServer::broadcastRing() {
    broadcast("doorRing");
}
