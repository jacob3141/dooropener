//
//  AppDelegate.swift
//  DOAR
//
//  Created by Sascha Schwabbauer on 22/12/14.
//  Copyright (c) 2014 evolved.io. All rights reserved.
//

import Cocoa
import DOARKitOSX

@NSApplicationMain
class AppDelegate: NSObject, NSApplicationDelegate {
    @IBOutlet weak var statusBarItemController: StatusBarItemController!
    
    func applicationDidFinishLaunching(aNotification: NSNotification) {
        NSNotificationCenter.defaultCenter().addObserver(self, selector: "didReceiveDoorRing:",
                name: AppConfiguration.Notifications.ConnectionDidReceiveDoorRingNotification,
                object: nil)
    }

    func applicationWillTerminate(aNotification: NSNotification) {
        // Insert code here to tear down your application
    }
    
    func appDelegateDidReceiveUserNotificationAction(notification: NSNotification) {
        openDoor()
    }
    
    func openDoor () {
        self.statusBarItemController.connectionController.openDoor()
    }
    
    // MARK: - Actions
    
    func didReceiveDoorRing(notification: NSNotification!) {
        showDoorRingNotification()
    }
    
    func showDoorRingNotification() {
        let userNotification = NSUserNotification()
        userNotification.title = NSLocalizedString("notification.door-ring.title", comment: "")
        userNotification.subtitle = NSLocalizedString("notification.door-ring.subtitle", comment: "")
        userNotification.actionButtonTitle = NSLocalizedString("notification.door-ring.action-button-title", comment: "")
        userNotification.soundName = "doorbell.m4a"
        
        NSUserNotificationCenter.defaultUserNotificationCenter().deliverNotification(userNotification)
    }

}

