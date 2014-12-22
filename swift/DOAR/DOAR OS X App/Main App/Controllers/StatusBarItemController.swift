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
    
    override init() {
        self.statusItem = NSStatusBar.systemStatusBar().statusItemWithLength(-1) // TOOD: Use NSVariableStatusItemLength if it works
        self.statusItem.button?.image = NSImage(named: "statusBarIcon")
        self.statusItem.button?.appearsDisabled = connectionController.state != .Open
        super.init()
    }
    
    override func awakeFromNib() {
        self.configureStatusItem()
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
        
    }
    
    func quit(sender: AnyObject?) {
        NSApplication.sharedApplication().terminate(self)
    }
    
    // MARK: - NSMenu
    
    override func validateMenuItem(menuItem: NSMenuItem) -> Bool {
        if menuItem == self.statusItem.menu?.itemArray.first as NSMenuItem {
            return connectionController.state == .Open
        }
        
        return true
    }
}
