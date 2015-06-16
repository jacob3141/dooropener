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
#include <QObject>
#include <QSemaphore>
#include <QTimer>

class DoorDriver : public QObject {
    Q_OBJECT
public:
    static DoorDriver& instance();
    ~DoorDriver();

signals:
    void opened();
    void closed();
    void ring();
    void error(QString message);

public slots:
    void open(int holdDuration = 3000);
    void simulateRing();

protected slots:
    void close();
    void ringPoll();

private:
    DoorDriver(QObject *parent = 0);

    QTimer *_openDoorHoldTimer;
    QTimer *_ringPollTimer;
    QSemaphore *_doorSemaphore;
};
