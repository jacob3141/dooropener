#
# This file is part of DoorOpener.
# Copyright (c) 2014 Jacob Dawid <jacob@omg-it.works>
#
# DoorOpener is free software: you can redistribute it and/or modify
# it under the terms of the GNU Affero General Public License as
# published by the Free Software Foundation, either version 3 of the
# License, or (at your option) any later version.
#
# DoorOpener is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Affero General Public License for more details.
#
# You should have received a copy of the GNU Affero General Public
# License along with DoorOpener.
# If not, see <http:www.gnu.org/licenses/>.
#

TEMPLATE = subdirs
SUBDIRS = qdoorserver qdoorclient \
    qdoordesktop

qdoorserver.subdir = qdoorserver
qdoorserver.depends =

qdoorclient.subdir = qdoorclient
qdoorclient.depends =

