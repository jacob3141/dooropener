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
#include <QCoreApplication>

// QtWebServer includes
#include "tcp/tcpmultithreadedserver.h"
#include "http/httpwebengine.h"

// Own includes
#include "websocketserver.h"
#include "indexresource.h"

int main(int argc, char *argv[]) {
    QCoreApplication application(argc, argv);

    // Configure websocket server
    WebSocketServer webSocketServer;
    webSocketServer.listen(QHostAddress::Any, 8080);

    // Configure webserver
    QtWebServer::Http::WebEngine webEngine;
    webEngine.addResource(new IndexResource());

    QtWebServer::Tcp::MultithreadedServer multithreadedWebServer;
    multithreadedWebServer.setResponder(&webEngine);
    multithreadedWebServer.listen(QHostAddress::Any, 80);

    // Run application
    return application.exec();
}
