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

// Own includes
#include "doordriver.h"

// Qt includes
#include <QFile>

#define DOOR_OPEN_IO "/sys/class/gpio/gpio41/value"
#define DOOR_RING_IO "/sys/class/gpio/gpio39/value"


DoorDriver& DoorDriver::instance() {
    static DoorDriver doorService;
    return doorService;
}

DoorDriver::DoorDriver(QObject *parent) :
    QObject(parent) {
    _openDoorHoldTimer = new QTimer(this);
    _openDoorHoldTimer->setSingleShot(true);
    connect(_openDoorHoldTimer, SIGNAL(timeout()),
            this, SLOT(turnOffDoorOpener()));

    _doorSemaphore = new QSemaphore(1);

    _ringPollTimer = new QTimer(this);
    _ringPollTimer->setInterval(50);
    connect(_ringPollTimer, SIGNAL(timeout()),
            this, SLOT(ringPoll()));
    _ringPollTimer->start();
}

DoorDriver::~DoorDriver() {
}

void DoorDriver::open(int holdDuration) {
    _doorSemaphore->acquire();
    if(!_openDoorHoldTimer->isActive()) {
        if(system(QString("echo 1 > %1").arg(DOOR_OPEN_IO).toStdString().c_str()) == 0) {
            emit opened();
            _openDoorHoldTimer->setInterval(holdDuration);
            _openDoorHoldTimer->start();

        }
    }
    _doorSemaphore->release();
}

void DoorDriver::close() {
    _doorSemaphore->acquire();
    if(system(QString("echo 0 > %1").arg(DOOR_OPEN_IO).toStdString().c_str()) == 0) {
        emit closed();
        _openDoorHoldTimer->stop();
    }
    _doorSemaphore->release();
}

void DoorDriver::ringPoll() {
    QFile file(DOOR_RING_IO);

    file.open(QFile::ReadOnly);
    if(file.isOpen()) {
        QByteArray contents = file.readAll();

        // Pulldown
        if(contents.contains("0")) {
            emit ring();
        }
        file.close();
    } else {
        emit error("Ring polling failed: " + file.errorString());
    }
}
