//
//  ViewController.swift
//  DOAR
//
//  Created by Sascha Schwabbauer on 22/12/14.
//  Copyright (c) 2014 evolved.io. All rights reserved.
//

import UIKit
import Starscream
import DOARKitiOS

class ViewController: UIViewController {
    
    @IBOutlet var statusLabel: UILabel!
    @IBOutlet var openDoorButton: UIButton!
    @IBOutlet var activityIndicator: UIActivityIndicatorView!
    
    var connectionController = ConnectionController()
    
    var connected: Bool = false {
        didSet {
            openDoorButton.enabled = connected;
            statusLabel.text = (connected) ? "Connected" : "Disconnected"
        }
    }

    override func viewDidLoad() {
        super.viewDidLoad()
        
        
    }

    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
        // Dispose of any resources that can be recreated.
    }
    
    /// MARK: Notifications
    
    private func registerForNotifications() {
//        NSNotificationCenter.defaultCenter().addObserver(self, selector: "connectionStateDidChange", name: AppConfiguration.Notifications.ConnectionStateDidChangeNotification, object: nil)
    }
    
    private func unregisterFromNotifications() {
//        NSNotificationCenter.defaultCenter().addObserver(self, selector: "connectionStateDidChange", name: AppConfiguration.Notifications.ConnectionDidReceiveDoorRingNotification, object: nil)
    }

    private func connectionStateDidChange() {
        switch (self.connectionController.state) {
        case .Closed:
            NSLog.("Connection closed", nil...)
        case .Failed:
            NSLog.("Connection failed", nil...)
        case .Open:
            NSLog.("Connection opened", nil...)
        }
    }
    
    private func doorRingReceived() {
        
    }

    /// MARK: Actions
    
    @IBAction func didTapOpenDoorButton(sender: UIButton) {
        openDoor()
    }
    
    /// MARK: Door handling
    
    private func openDoor() {
        self.activityIndicator.startAnimating()
        
        // TODO: Actually open door
        
        self.activityIndicator.stopAnimating()
    }
    
}

