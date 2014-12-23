//
//  StatusBarItemController.swift
//  DOAR
//
//  Created by Sascha Schwabbauer on 22/12/14.
//  Copyright (c) 2014 evolved.io. All rights reserved.
//

import Cocoa
import DOARKitOSX

class StatusBarItemController: NSObject, NSUserNotificationCenterDelegate {
    enum OpenDoorState {
        case Idle
        case Opening
    }
    
    let connectionController = ConnectionController()
    private let statusItem: NSStatusItem
    private var reconnectTimer: NSTimer?
    private var state = OpenDoorState.Idle
    
    override init() {
        self.statusItem = NSStatusBar.systemStatusBar().statusItemWithLength(-1) // TOOD: Use NSVariableStatusItemLength if it works
        self.statusItem.button?.image = NSImage(named: "statusBarIcon")
        self.statusItem.button?.appearsDisabled = self.connectionController.state != .Open
        super.init()
    }
    
    deinit {
        NSNotificationCenter.defaultCenter().removeObserver(self)
        NSUserNotificationCenter.defaultUserNotificationCenter().delegate = nil
    }
    
    override func awakeFromNib() {
        self.configureStatusItem()
        self.connectionController.connect()
        NSUserNotificationCenter.defaultUserNotificationCenter().delegate = self
        
        NSNotificationCenter.defaultCenter().addObserver(self, selector: "didReceiveDoorRing:",
            name: AppConfiguration.Notifications.ConnectionDidReceiveDoorRingNotification,
            object: nil)
        
        NSNotificationCenter.defaultCenter().addObserverForName(AppConfiguration.Notifications.ConnectionStateDidChangeNotification, object: nil, queue: NSOperationQueue.mainQueue()) { (note) -> Void in
            self.statusItem.menu?.update()
            self.statusItem.button?.appearsDisabled = self.connectionController.state != .Open
            
            if self.connectionController.state != .Open {
                NSUserNotificationCenter.defaultUserNotificationCenter().removeAllDeliveredNotifications()
                self.createReconnectTimer()
            } else {
                self.reconnectTimer?.invalidate()
                self.reconnectTimer = nil
            }
        }
        
        NSNotificationCenter.defaultCenter().addObserverForName(AppConfiguration.Notifications.ConnectionDidReceiveWillOpenDoorNotification, object: nil, queue: NSOperationQueue.mainQueue()) { (note) -> Void in
            NSUserNotificationCenter.defaultUserNotificationCenter().removeAllDeliveredNotifications()
            self.state = .Opening
        }
        
        NSNotificationCenter.defaultCenter().addObserverForName(AppConfiguration.Notifications.ConnectionDidReceiveDidOpenDoorNotification, object: nil, queue: NSOperationQueue.mainQueue()) { (note) -> Void in
            self.state = .Idle
        }
    }
    
    // MARK: - Notification callbacks
    
    func didReceiveDoorRing(notification: NSNotification!) {
        if self.state == .Opening {
            return
        }
        
        NSUserNotificationCenter.defaultUserNotificationCenter().removeAllDeliveredNotifications()
        
        let userNotification = NSUserNotification()
        userNotification.title = NSLocalizedString("notification.door-ring.title", comment: "")
        userNotification.subtitle = NSLocalizedString("notification.door-ring.subtitle", comment: "")
        userNotification.actionButtonTitle = NSLocalizedString("notification.door-ring.action-button-title", comment: "")
        userNotification.soundName = "doorbell.m4a"
        NSUserNotificationCenter.defaultUserNotificationCenter().deliverNotification(userNotification)
    }
    
    // MARK: - Helpers
    
    private func configureStatusItem() {
        let menu = NSMenu()
        
        let openDoorItem = NSMenuItem(title: NSLocalizedString("menu.open-door", comment: ""), action: "openDoor:", keyEquivalent: "")
        openDoorItem.target = self
        menu.addItem(openDoorItem)
        
        var quitMenuItem = NSMenuItem(title: NSLocalizedString("menu.quit", comment: ""), action: "quit:", keyEquivalent: "")
        quitMenuItem.target = self
        menu.addItem(quitMenuItem)
        
        self.statusItem.menu = menu
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
    
    func openDoor(sender: AnyObject?) {
        self.connectionController.openDoor()
    }
    
    func quit(sender: AnyObject?) {
        NSApplication.sharedApplication().terminate(self)
    }
    
    // MARK: - NSMenu
    
    override func validateMenuItem(menuItem: NSMenuItem) -> Bool {
        if menuItem == self.statusItem.menu?.itemArray.first as NSMenuItem {
            return self.connectionController.state == .Open && self.state == .Idle
        }
        
        return true
    }
    
    // MARK: - NSUserNotificationCenterDelegate
    
    func userNotificationCenter(center: NSUserNotificationCenter, shouldPresentNotification notification: NSUserNotification) -> Bool {
        return true
    }
    
    func userNotificationCenter(center: NSUserNotificationCenter, didActivateNotification notification: NSUserNotification) {
        self.openDoor(nil)
    }
}
