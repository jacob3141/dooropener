//
//  InterfaceController.swift
//  DOAR WatchKit Extension
//
//  Created by Sascha Schwabbauer on 22/12/14.
//  Copyright (c) 2014 evolved.io. All rights reserved.
//

import WatchKit
import Foundation
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
    private var state: OpenDoorState = .Idle {
        didSet {
            self.openDoorButton.setEnabled(self.state == .Idle)
        }
    }
    
    // MARK: - WKInterfaceController
    
    override init!() {
        super.init()
        self.createObservers()
    }
    
    deinit {
        NSNotificationCenter.defaultCenter().removeObserver(self)
    }
    
    override func awakeWithContext(context: AnyObject?) {
        super.awakeWithContext(context)
    }
    
    override func willActivate() {
        self.connectionController.connect()
        super.willActivate()
    }
    
    override func didDeactivate() {
        self.connectionController.disconnect()
        super.didDeactivate()
    }
    
    // MARK: - Helpers
    
    private func createObservers() {
        NSNotificationCenter.defaultCenter().addObserver(self, selector: "connectionStateDidChange:", name: AppConfiguration.Notifications.ConnectionStateDidChangeNotification, object: nil)
        
        NSNotificationCenter.defaultCenter().addObserver(self, selector: "willOpenDoor:", name: AppConfiguration.Notifications.ConnectionDidReceiveWillOpenDoorNotification, object: nil)
        
        NSNotificationCenter.defaultCenter().addObserver(self, selector: "didOpenDoor:", name: AppConfiguration.Notifications.ConnectionDidReceiveDidOpenDoorNotification, object: nil)
    }
    
    private func createReconnectTimer() {
        if self.reconnectTimer == nil {
            self.reconnectTimer = NSTimer.scheduledTimerWithTimeInterval(10, target: self, selector: "reconnect:", userInfo: nil, repeats: false)
        }
    }
    
    func reconnect(timer: NSTimer) {
        self.reconnectTimer = nil
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
    
    func willOpenDoor(note: NSNotification?) {
        self.state = .Opening
    }
    
    func didOpenDoor(note: NSNotification?) {
        self.state = .Idle
    }
    
    @IBAction func openDoor() {
        if self.connectionController.state == .Open {
            self.connectionController.openDoor()
        }
    }
    
}
