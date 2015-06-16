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

// QtWebServer includes
#include "util/utildataurlcodec.h"
#include "util/utilformurlcodec.h"

// Own includes
#include "indexresource.h"
#include "doordriver.h"

IndexResource::IndexResource() :
    QtWebServer::Http::Resource("/") {
}

IndexResource::~IndexResource() {
}

void IndexResource::deliver(const QtWebServer::Http::Request& request,
                            QtWebServer::Http::Response& response) {

    response.setStatusCode(QtWebServer::Http::Ok);
    response.setHeader(QtWebServer::Http::ContentType, "text/html");

    QImage catImage = QImage(":/cat-opening-door-handle.png");
    QByteArray urlEncodedCatImage = QtWebServer::Util::DataUrlCodec::dataUrlFromImage(catImage, "png");

    QString body = QString(
        "<img src=\"%1\" />"
        "<form action=\"/\" method=\"post\">"
            "<button type=\"submit\" name=\"action\" value=\"open\">"
                "Open door"
            "</button>"
            "<button type=\"submit\" name=\"action\" value=\"simulate-ring\">"
                "Simulate ring"
            "</button>"
        "</form>")
        .arg(QString::fromUtf8(urlEncodedCatImage));

    if(request.method() == "post") {
        QMap<QString, QByteArray> formParameters
            = QtWebServer::Util::FormUrlCodec::decodeFormUrl(request.body());

        if(formParameters["action"] == "open") {
            DoorDriver::instance().open();

            body += "<p><b>The door has been opened.</b></p>";
            response.setHeader(QtWebServer::Http::Refresh, "2");
        } else if(formParameters["action"] == "simulate-ring") {
            DoorDriver::instance().simulateRing();

            body += "<p><b>Ring simulated.</b></p>";
            response.setHeader(QtWebServer::Http::Refresh, "2");
        }
    }

    response.setBody(body.toUtf8());
}
