//
//  InterfaceController.swift
//  DOAR WatchKit Extension
//
//  Created by Sascha Schwabbauer on 22/12/14.
//  Copyright (c) 2014 evolved.io. All rights reserved.
//

import WatchKit
import Foundation
import Starscream
import DOARKitiOS

class InterfaceController: WKInterfaceController {
    enum OpenDoorState {
        case Idle
        case Opening
    }
    
    @IBOutlet var stateLabel: WKInterfaceLabel!
    @IBOutlet var openDoorButton: WKInterfaceButton!
    let connectionController = ConnectionController()
    private var reconnectTimer: NSTimer?
    private var state = OpenDoorState.Idle
    
   // MARK: - WKInterfaceController
    
    override init!() {
        super.init()
        self.createObservers()
        self.connectionController.connect()
    }
    
    override func awakeWithContext(context: AnyObject?) {
        super.awakeWithContext(context)
    }

    override func willActivate() {
        super.willActivate()
    }

    override func didDeactivate() {
        super.didDeactivate()
    }

    // MARK: - Helpers
    
    private func createObservers() {
        NSNotificationCenter.defaultCenter().addObserver(self, selector: "connectionStateDidChange:", name: AppConfiguration.Notifications.ConnectionStateDidChangeNotification, object: nil)
        
        NSNotificationCenter.defaultCenter().addObserverForName(AppConfiguration.Notifications.ConnectionDidReceiveWillOpenDoorNotification, object: nil, queue: NSOperationQueue.mainQueue()) { (note) -> Void in
            self.state = .Opening
        }
        
        NSNotificationCenter.defaultCenter().addObserverForName(AppConfiguration.Notifications.ConnectionDidReceiveDidOpenDoorNotification, object: nil, queue: NSOperationQueue.mainQueue()) { (note) -> Void in
            self.state = .Idle
        }
    }
    
    private func createReconnectTimer() {
        if self.reconnectTimer == nil {
            self.reconnectTimer = NSTimer.scheduledTimerWithTimeInterval(10, target: self, selector: "reconnect:", userInfo: nil, repeats: false)
        }
    }
    
    func reconnect(timer: NSTimer) {
        self.connectionController.connect()
    }
    
    // MARK: - Actions
    
    func connectionStateDidChange(note: NSNotification?) {
        if self.connectionController.state != .Open {
            self.stateLabel.setText(NSLocalizedString("label.state.disconnected", comment: ""))
            self.openDoorButton.setEnabled(false)
            self.createReconnectTimer()
        } else {
            self.stateLabel.setText(NSLocalizedString("label.state.connected", comment: ""))
            self.openDoorButton.setEnabled(true)
            self.reconnectTimer?.invalidate()
            self.reconnectTimer = nil
        }
    }
    
    @IBAction func openDoor() {
        self.connectionController.openDoor()
    }
    
}
