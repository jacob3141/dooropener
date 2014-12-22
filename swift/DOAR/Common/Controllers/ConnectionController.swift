//
//  ConnectionController.swift
//  DOAR
//
//  Created by Sascha Schwabbauer on 22/12/14.
//  Copyright (c) 2014 evolved.io. All rights reserved.
//

import Foundation
#if os(OSX)
    import StarscreamOSX
#elseif os(iOS)
    import Starscream
#endif

public enum ConnectionState {
    case Closed
    case Open
    case Failed
}

public class ConnectionController: WebSocketDelegate {
    private let socket = WebSocket(url: NSURL(scheme: AppConfiguration.WebSocketURL.proto, host: AppConfiguration.WebSocketURL.host, path: AppConfiguration.WebSocketURL.path)!)
    
    public private(set) var state: ConnectionState = ConnectionState.Closed {
        didSet {
            NSNotificationCenter.defaultCenter().postNotificationName(AppConfiguration.Notifications.ConnectionStateDidChangeNotification, object: self)
        }
    }
    
    public init() {
        
    }
    
    // MARK: - Public API
    
    public func connect() {
        self.socket.delegate = self
        self.socket.connect()
    }
    
    public func openDoor() {
        self.socket.writeString(AppConfiguration.ServerCommands.openDoor)
    }
    
    public func close() {
        self.socket.disconnect()
    }
    
    // MARK: - WebSocketDelegate
    
    public func websocketDidConnect() {
        self.state = ConnectionState.Open
    }
    
    public func websocketDidDisconnect(error: NSError?) {
        if let error = error {
            self.state = ConnectionState.Failed
        } else {
            self.state = ConnectionState.Closed
        }
    }
    
    public func websocketDidReceiveMessage(text: String) {
        if text == AppConfiguration.ServerCommands.doorRing {
            NSNotificationCenter.defaultCenter().postNotificationName(AppConfiguration.Notifications.ConnectionDidReceiveDoorRingNotification, object: self)
        } else if text ==  AppConfiguration.ServerCommands.willOpenDoor {
            
        } else {
            println("Received unknown message from server: \(text)")
        }
    }
    
    public func websocketDidWriteError(error: NSError?) {
        
    }
    
    public func websocketDidReceiveData(data: NSData) {
        
    }
}
