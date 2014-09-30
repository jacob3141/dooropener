DoorOpener
==========

Client and server that can be used as a door opener system.
Architecture overview:
![Architecture](https://github.com/cybercatalyst/dooropener/blob/master/dooropener.png "Architecture")

DoorOpener has been developed after an idea at 9elements on one of our hackathons. For the mere hacking pleasure we wanted to write our own system to open the door with our mobile phones and computers. I have written a Qt app that runs on all mobile and desktop platforms some time later, and have rewritten the server component with Qt.

Hardware
========

You will need
- A small server (for example a linux thin client)
- An arduino that will read commands via serial port and trigger relays on a power board

Setup
=====
Create a "door.json", you can take the "door.json.example" as a starting point. This file has to be placed next to the server binary or under /etc/door/. It is recommended that you set up the server as a service.
License
=======
DoorOpener is free software license under AGPLv3.
