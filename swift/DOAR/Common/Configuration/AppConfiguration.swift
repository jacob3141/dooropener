//
//  AppConfiguration.swift
//  DOAR
//
//  Created by Sascha Schwabbauer on 22/12/14.
//  Copyright (c) 2014 evolved.io. All rights reserved.
//

import Foundation

public struct AppConfiguration {
    internal struct WebSocketURL {
        static let proto = "ws"
        static let host = "192.168.100.30:8080"
        static let path = "/"
    }
    
    internal struct ServerCommands {
        static let openDoor = "openDoor"
        static let doorRing = "doorRing"
        static let willOpenDoor = "willOpenDoor"
        static let didOpenDoor = "didOpenDoor"
    }
    
    public struct Notifications {
        public static let ConnectionStateDidChangeNotification = "ConnectionStateDidChangeNotification"
        public static let ConnectionDidReceiveDoorRingNotification = "ConnectionDidReceiveDoorRingNotification"
        public static let ConnectionDidReceiveWillOpenDoorNotification = "ConnectionDidReceiveWillOpenDoorNotification"
        public static let ConnectionDidReceiveDidOpenDoorNotification = "ConnectionDidReceiveDidOpenDoorNotification"
    }
}
