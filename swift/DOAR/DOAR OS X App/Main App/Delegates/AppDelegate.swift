//
//  AppDelegate.swift
//  DOAR
//
//  Created by Sascha Schwabbauer on 22/12/14.
//  Copyright (c) 2014 evolved.io. All rights reserved.
//

import Cocoa
import StarscreamOSX
import DOARKitOSX

@NSApplicationMain
class AppDelegate: NSObject, NSApplicationDelegate {
    @IBOutlet weak var statusMenu: NSMenu!
    let connectionController = ConnectionController()
    var statusItem: NSStatusItem?
    
    override init() {
        super.init()
    }

    func applicationDidFinishLaunching(aNotification: NSNotification) {
        connectionController.connect()
        
        self.statusItem = NSStatusBar.systemStatusBar().statusItemWithLength(-1)
        self.statusItem!.menu = self.statusMenu
        self.statusItem!.title = "DOAR"
        self.statusItem!.highlightMode = true
    }

    func applicationWillTerminate(aNotification: NSNotification) {
        // Insert code here to tear down your application
    }
    
    @IBAction func openDoor(sender: AnyObject?) {
        self.connectionController.openDoor()
    }

}

