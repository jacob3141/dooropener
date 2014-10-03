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

// Qt includes
#include <QDebug>
#include <QFileInfo>
#include <QDir>
#include <QWebSocket>
#include <QJsonDocument>
#include <QJsonObject>
#include <QBuffer>

// Own includes
#include "dooropenerservice.h"

DoorOpenerService::DoorOpenerService(QObject *parent)
    : QWebSocketServer("DoorServer", QWebSocketServer::NonSecureMode, parent)
{
    _serialPort = new QSerialPort();
    _serialPort->setBaudRate(9600);

    _camera = new QCamera();
    _cameraFrameGrabber = new CameraFrameGrabber();
    _camera->setViewfinder(_cameraFrameGrabber);

    connect(_camera, SIGNAL(error(QCamera::Error)), this, SLOT(cameraError(QCamera::Error)));
    connect(_camera, SIGNAL(statusChanged(QCamera::Status)), this, SLOT(cameraStatusChanged(QCamera::Status)));
    connect(_cameraFrameGrabber, SIGNAL(frameAvailable(QImage)), this, SLOT(backupCameraFrame(QImage)));
    _camera->start();

    _frameSendTimer = new QTimer();
    _frameSendTimer->setInterval(500);
    connect(_frameSendTimer, SIGNAL(timeout()), this, SLOT(sendCameraFrame()));
    _frameSendTimer->start();

    // Time between device discoveries
    _deviceDiscoveryTimer = new QTimer();
    _deviceDiscoveryTimer->setSingleShot(true);
    connect(_deviceDiscoveryTimer, SIGNAL(timeout()), this, SLOT(discoverDevices()));

    // Timer hold for opening door
    _openDoorHoldTimer = new QTimer();
    _openDoorHoldTimer->setSingleShot(true);
    connect(_openDoorHoldTimer, SIGNAL(timeout()), this, SLOT(turnOffDoorOpener()));

    connect(_serialPort,
            SIGNAL(readyRead()),
            this,
            SLOT(dataReceivedOnSerial()));
    connect(_serialPort,
            SIGNAL(error(QSerialPort::SerialPortError)),
            this,
            SLOT(handleSerialError(QSerialPort::SerialPortError)));

    connect(this,
            SIGNAL(newConnection()),
            this,
            SLOT(handlePendingConnections()));

    _potentialConfigurationFiles << "door.json" << "/etc/door/door.json";
    _configurationFilesWatcher.addPaths(_potentialConfigurationFiles);
    connect(&_configurationFilesWatcher, SIGNAL(fileChanged(QString)), this, SLOT(updateConfiguration(QString)));

    updateConfiguration();
    startService();
}

void DoorOpenerService::handlePendingConnections()
{
    while(hasPendingConnections()) {
        qDebug() << "Incoming connection";
        QWebSocket *clientSocket = nextPendingConnection();
        connect(clientSocket,
                SIGNAL(disconnected()),
                this,
                SLOT(clientDisconnected()));

        connect(clientSocket,
                SIGNAL(textMessageReceived(QString)),
                this,
                SLOT(clientMessageReceived(QString)));

        connect(clientSocket,
                SIGNAL(error(QAbstractSocket::SocketError)),
                this,
                SLOT(clientErrorOccurred(QAbstractSocket::SocketError)));
        _connectedClients.append(clientSocket);
    }
}

void DoorOpenerService::clientDisconnected()
{
    qDebug() << "Client disconnected";
    QWebSocket* clientSocket = dynamic_cast<QWebSocket*>(sender());
    if(!clientSocket) {
        return;
    }
    _connectedClients.removeAll(clientSocket);
    clientSocket->deleteLater();
}

void DoorOpenerService::clientMessageReceived(QString message)
{
    QWebSocket* clientSocket = dynamic_cast<QWebSocket*>(sender());
    if(!clientSocket) {
        return;
    }

    qDebug() << "Client " << (long)clientSocket <<": " << message;

    if(message.contains(_configuration["server"].toObject()["openDoorCommand"].toString())) {
        turnOnDoorOpener();
    }
}

void DoorOpenerService::clientErrorOccurred(QAbstractSocket::SocketError error)
{
    qDebug() << "Client error occurred";
    Q_UNUSED(error);
    QWebSocket* clientSocket = dynamic_cast<QWebSocket*>(sender());
    if(!clientSocket) {
        return;
    }
    // Try to close connection normally.
    clientSocket->close();

    // In any case, remove client.
    _connectedClients.removeAll(clientSocket);
    clientSocket->deleteLater();
}

void DoorOpenerService::sendBroadcast(QString message)
{
    foreach(QWebSocket* webSocket, _connectedClients) {
        webSocket->sendTextMessage(message);
    }
}

void DoorOpenerService::backupCameraFrame(QImage image)
{
    _frame = image;
}

void DoorOpenerService::sendCameraFrame()
{
    if(_frame.isNull())
        return;
    QBuffer buffer;
    QImage image = _frame.scaled(160, 120);
    image.save(&buffer, "PNG", 0);
    QString base64EncodedImage = QString::fromUtf8(buffer.data().toBase64());
    sendBroadcast(QString("image %1").arg(base64EncodedImage));
}

void DoorOpenerService::cameraError(QCamera::Error error)
{
    switch(error) {
    case QCamera::NoError: qDebug() << "Camera: OK"; break;
    case QCamera::CameraError: qDebug() << "Camera: unknown error"; break;
    case QCamera::InvalidRequestError: qDebug() << "Camera: invalid request"; break;
    case QCamera::ServiceMissingError: qDebug() << "Camera: service missing"; break;
    case QCamera::NotSupportedFeatureError: qDebug() << "Camera: not supported feature"; break;
    }
}

void DoorOpenerService::cameraStatusChanged(QCamera::Status status)
{
    switch(status) {
    case QCamera::UnavailableStatus: qDebug() << "Camera unavailable"; break;
    case QCamera::UnloadedStatus: qDebug() << "Camera unloaded"; break;
    case QCamera::LoadingStatus: qDebug() << "Camera loading"; break;
    case QCamera::UnloadingStatus: qDebug() << "Camera unloading"; break;
    case QCamera::LoadedStatus: qDebug() << "Camera loaded"; break;
    case QCamera::StandbyStatus: qDebug() << "Camera standby"; break;
    case QCamera::StartingStatus: qDebug() << "Camera starting"; break;
    case QCamera::StoppingStatus: qDebug() << "Camera stopping"; break;
    case QCamera::ActiveStatus: qDebug() << "Camera active"; break;
    }

    if(status == QCamera::ActiveStatus) {
        qDebug() << "Starting camera capture";
    }
}

void DoorOpenerService::discoverDevices()
{
    _deviceDiscoveryTimer->stop();
    QDir dir("/dev");
    QStringList nameFilters;
    nameFilters << "ttyUSB*";
    QStringList deviceList = dir.entryList(nameFilters, QDir::System);
    if(!deviceList.isEmpty()) {
        QString deviceName = deviceList.at(0);
        qDebug() << "Connecting to device: " << deviceName;
        _serialPort->setPortName(deviceName);
        establishSerialConnection();
    } else {
        closeSerialConnection();
    }
}

void DoorOpenerService::establishSerialConnection()
{
    _serialPort->open(QSerialPort::ReadWrite);
    if(_serialPort->isOpen()) {
        qDebug() << "Established connection to serial device:" << _serialPort->portName();
    } else {
        qDebug() << "Could not open serial port:" << _serialPort->errorString();
        _deviceDiscoveryTimer->start();
    }
}

void DoorOpenerService::handleSerialError(QSerialPort::SerialPortError error)
{
    switch(error) {
    // Will occur when device is being removed unexpectedly
    case QSerialPort::ResourceError:
        qDebug() << "Device error. Check connection cable.";
        closeSerialConnection();
        break;
    default:
        break;
    }
}

void DoorOpenerService::closeSerialConnection()
{
    _serialPort->close();
    _deviceDiscoveryTimer->start();
}

void DoorOpenerService::dataReceivedOnSerial()
{
    if(_serialPort->canReadLine()) {
        QByteArray byteArray = _serialPort->readLine();
        QString messageReceived = QString::fromUtf8(byteArray);
        qDebug() << "Received data on serial port: " << messageReceived;

        if(messageReceived.contains("bell")) {
            sendBroadcast("doorRing");
        }
    }
}

void DoorOpenerService::sendDataOnSerial(QString data)
{
    qDebug() << "Writing data to serial port: " << data;
    _serialPort->write(QString("%1%2").arg(data).arg("\n").toUtf8());
}

void DoorOpenerService::turnOnDoorOpener()
{
    sendBroadcast(_configuration["server"].toObject()["willOpenDoorCommand"].toString());
    // Do not attempt to open door multiple times
    if(!_openDoorHoldTimer->isActive()) {
        _openDoorHoldTimer->start();
        sendDataOnSerial(_configuration["serial"].toObject()["openDoorCommand"].toString());
    }
}

void DoorOpenerService::turnOffDoorOpener()
{
    sendDataOnSerial(_configuration["serial"].toObject()["closeDoorCommand"].toString());
    sendBroadcast(_configuration["server"].toObject()["didOpenDoorCommand"].toString());
}

void DoorOpenerService::startService()
{
    _deviceDiscoveryTimer->setInterval(_configuration["serial"].toObject()["deviceDiscoveryDelay"].toInt());
    _openDoorHoldTimer->setInterval(_configuration["serial"].toObject()["openDoorHoldTime"].toInt());

    int port = _configuration["server"].toObject()["port"].toInt();
    if(!listen(QHostAddress::Any, port)) {
        qDebug() << "Could not open server. Reason: " << errorString();
    } else {
        qDebug() << "Listening on port: " << port;
    }

    _deviceDiscoveryTimer->start();
}

void DoorOpenerService::stopService()
{
    close();
}

void DoorOpenerService::resetService()
{
    stopService();
    startService();
}

void DoorOpenerService::updateConfiguration(QString file)
{
    Q_UNUSED(file);
    foreach(QString potentialConfigurationFile, _potentialConfigurationFiles) {
        QFile file(potentialConfigurationFile);
        if(file.exists(potentialConfigurationFile)) {
            file.open(QFile::ReadOnly);
            if(file.isOpen()) {
                qDebug() << "Reading configuration from:" << potentialConfigurationFile;
                QJsonParseError error;
                QJsonDocument jsonDocument = QJsonDocument::fromJson(file.readAll(), &error);
                _configuration = jsonDocument.object();
                file.close();
                if(_configuration.isEmpty()) {
                    qDebug() << "Error parsing configuration file: " << error.errorString();
                } else {
                    qDebug() << jsonDocument.toJson();
                }
                return;
            } else {
                qDebug() << "Could not open configuration file for read:" << potentialConfigurationFile;
            }
        } else {
            qDebug() << "File" << potentialConfigurationFile << " does not exist.";
        }
    }

    qDebug() << "Warning: No configuration files found. Server will not work.";
}

