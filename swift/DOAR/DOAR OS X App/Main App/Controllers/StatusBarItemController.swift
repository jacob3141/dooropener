//
//  StatusBarItemController.swift
//  DOAR
//
//  Created by Sascha Schwabbauer on 22/12/14.
//  Copyright (c) 2014 evolved.io. All rights reserved.
//

import Cocoa
import StarscreamOSX
import DOARKitOSX

class StatusBarItemController: NSObject {
    private let connectionController = ConnectionController()
    private let statusItem: NSStatusItem
    private var reconnectTimer: NSTimer?
    
    override init() {
        self.statusItem = NSStatusBar.systemStatusBar().statusItemWithLength(-1) // TOOD: Use NSVariableStatusItemLength if it works
        self.statusItem.button?.image = NSImage(named: "statusBarIcon")
        self.statusItem.button?.appearsDisabled = self.connectionController.state != .Open
        super.init()
    }
    
    override func awakeFromNib() {
        self.configureStatusItem()
        self.connectionController.connect()
        
        NSNotificationCenter.defaultCenter().addObserverForName(AppConfiguration.Notifications.ConnectionStateDidChangeNotification, object: nil, queue: NSOperationQueue.mainQueue()) { (note) -> Void in
            self.statusItem.menu?.update()
            self.statusItem.button?.appearsDisabled = self.connectionController.state != .Open
            
            if self.connectionController.state != .Open {
                self.createReconnectTimer()
            } else {
                self.reconnectTimer?.invalidate()
                self.reconnectTimer = nil
            }
        }
    }
    
    private func createReconnectTimer() {
        if self.reconnectTimer == nil {
            self.reconnectTimer = NSTimer.scheduledTimerWithTimeInterval(10, target: self, selector: Selector("reconnect:"), userInfo: nil, repeats: false)
        }
    }
    
    private func reconnect(timer: NSTimer) {
        self.connectionController.connect()
    }
    
    private func configureStatusItem() {
        let menu = NSMenu()
        
        let openDoorItem = NSMenuItem(title: NSLocalizedString("menu.open-door", comment: ""), action: Selector("openDoor:"), keyEquivalent: "")
        openDoorItem.target = self
        menu.addItem(openDoorItem)
        
        var quitMenuItem = NSMenuItem(title: NSLocalizedString("menu.quit", comment: ""), action: Selector("quit:"), keyEquivalent: "")
        quitMenuItem.target = self
        menu.addItem(quitMenuItem)
        
        self.statusItem.menu = menu
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
            return self.connectionController.state == .Open
        }
        
        return true
    }
}
