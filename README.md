DoorOpener
==========

Client and server that can be used as a door opener system.
Architecture overview:
![Architecture](https://github.com/cybercatalyst/dooropener/blob/master/dooropener.png "Architecture")

DoorOpener has been developed after an idea at 9elements on one of our hackathons. For the mere hacking pleasure we wanted to write our own system to open the door with our mobile phones and computers. I have written a Qt app that runs on all mobile and desktop platforms some time later, and have rewritten the server component with Qt.
![Android app](https://github.com/cybercatalyst/dooropener/blob/master/android.png "Android app")

Hardware
========

For the server you will need an Olimex OLinuXino A20 LIME.

Setup
=====
Create a "door.json", you can take the "door.json.example" as a starting point. This file has to be placed next to the server binary or under /etc/door/. It is recommended that you set up the server as a service.

Known problems
==============
- QT: Unknown module serialport
The server and client application, as well as the arduino source code are surrounded by a subdirs project. This is perfectly fine as long as you are building the client and server on the desktop. If you are trying to configure everything for ARMv7 (mobile devices), qmake/QtCreator will try to build both projects for mobile, which does not work. On mobile devices, there is no support for the serialport module of Qt. For some strange reason, QtCreator refuses to build any of the projects then, even if you only build the client. The solution is to open the client as an independet by opening qdoorclient.pro in qdoorclient/ with QtCreator and configure the project for ARMv7 there. 

License
=======
DoorOpener is free software license under AGPLv3. If you have any questions, just drop me a mail at: jacob@omg-it.works.
