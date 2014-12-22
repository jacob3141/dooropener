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

#ifndef DOOROPENERSERVICE_H
#define DOOROPENERSERVICE_H

// Qt includes
#include <QWebSocketServer>
#include <QTimer>
#include <QList>
#include <QJsonObject>
#include <QFileSystemWatcher>
#include <QCamera>

// Own includes
#include "cameraframegrabber.h"

class DoorOpenerService : public QWebSocketServer
{
    Q_OBJECT
public:
    DoorOpenerService(QObject *parent = 0);

public slots:
    ///////////////////////////////////////////////////////
    // Web interface
    ///////////////////////////////////////////////////////

    /** Check for any pending connections and accept them. */
    void handlePendingConnections();
    /** Disconnect client. */
    void clientDisconnected();
    /** Handle message received from client. */
    void clientMessageReceived(QString message);
    /** Handle errors occurred with a client. */
    void clientErrorOccurred(QAbstractSocket::SocketError error);
    /** Send a broadcast message to all clients. */
    void sendBroadcast(QString message);

    void backupCameraFrame(QImage image);
    void sendCameraFrame();
    void cameraError(QCamera::Error error);
    //void cameraCaptureError(int id, QCameraImageCapture::Error error ,QString errorString);
    void cameraStatusChanged(QCamera::Status status);

    /** Perform door open action. */
    void turnOnDoorOpener();
    /** Perform door close action. */
    void turnOffDoorOpener();

    /** Update server configuration from available files. */
    void updateConfiguration(QString file = "");

private:
    void startService();
    void stopService();
    void resetService();
    void initialStartup();

    QList<QWebSocket*> _connectedClients;
    QTimer *_openDoorHoldTimer;

    QImage _frame;
    QCamera *_camera;
    CameraFrameGrabber *_cameraFrameGrabber;
    QTimer *_frameSendTimer;

    QJsonObject _configuration;
    QStringList _potentialConfigurationFiles;
    QFileSystemWatcher _configurationFilesWatcher;
};

#endif // DOOROPENERSERVICE_H
