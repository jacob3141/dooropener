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

#pragma once

// Qt includes
#include <QWebSocketServer>
#include <QList>
#include <QSemaphore>

class WebSocketServer :
    public QWebSocketServer {
    Q_OBJECT
public:
    WebSocketServer(QObject *parent = 0);

    /** Send a broadcast message to all clients. */
    void broadcast(QString message);

public slots:
    /** Check for any pending connections and accept them. */
    void handlePendingConnections();
    /** Disconnect client. */
    void clientDisconnected();
    /** Handle message received from client. */
    void clientMessageReceived(QString message);
    /** Handle errors occurred with a client. */
    void clientErrorOccurred(QAbstractSocket::SocketError error);

private slots:
    void sendOpenCommand();
    void broadcastDoorOpened();
    void broadcastDoorClosed();
    void broadcastRing();

private:
    QSemaphore *_connectedClientsSemaphore;
    QList<QWebSocket*> _connectedClients;
};
