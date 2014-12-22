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
import AVFoundation

class ViewController: UIViewController {
    
    @IBOutlet var statusLabel: UILabel!
    @IBOutlet var openDoorButton: UIButton!
    @IBOutlet var activityIndicator: UIActivityIndicatorView!
    @IBOutlet var ringImageView: UIImageView!
    
    var audioPlayer: AVAudioPlayer?
    
    var connectionController = ConnectionController()
    
    var connected: Bool = false {
        didSet {
            openDoorButton.enabled = connected;
            statusLabel.text = (connected) ? "Connected" : "Disconnected"
        }
    }

    override func viewDidLoad() {
        super.viewDidLoad()
        
        self.connectionController.connect()
        registerForNotifications()
    }
    
    deinit {
        unregisterFromNotifications()
    }
    
    /// MARK: Notifications
    
    private func registerForNotifications() {
        NSNotificationCenter.defaultCenter().addObserver(self, selector: "connectionStateDidChange", name: AppConfiguration.Notifications.ConnectionStateDidChangeNotification, object: nil)
        NSNotificationCenter.defaultCenter().addObserver(self, selector: "doorRingReceived", name: AppConfiguration.Notifications.ConnectionDidReceiveDoorRingNotification, object: nil)
        NSNotificationCenter.defaultCenter().addObserver(self, selector: "didOpenDoorReceived", name: AppConfiguration.Notifications.ConnectionDidReceiveOpenDoorNotification, object: nil)
    }
    
    func unregisterFromNotifications() {
        let notificationCenter = NSNotificationCenter.defaultCenter()
        notificationCenter.removeObserver(self, name: AppConfiguration.Notifications.ConnectionStateDidChangeNotification, object: nil)
        notificationCenter.removeObserver(self, name: AppConfiguration.Notifications.ConnectionDidReceiveDoorRingNotification, object: nil)
        notificationCenter.removeObserver(self, name: AppConfiguration.Notifications.ConnectionDidReceiveOpenDoorNotification, object: nil)
    }

    func connectionStateDidChange() {
        switch (self.connectionController.state) {
        case .Closed:
            self.connected = false
        case .Failed:
            self.connected = false
        case .Open:
            self.connected = true
        }
    }
    
    func doorRingReceived() {
        let path = NSBundle.mainBundle().pathForResource("doorbell", ofType: "m4a")
        let url = NSURL(string: path!)
        
        var error:NSError?
        self.audioPlayer = AVAudioPlayer(contentsOfURL: url, error: &error)
        self.audioPlayer!.numberOfLoops = 3
        self.audioPlayer!.prepareToPlay()
        self.audioPlayer!.play()
        
        startAnimatingRing()
    }

    func didOpenDoorReceived() {
        self.activityIndicator.stopAnimating()
        self.openDoorButton.enabled = true
        
        UIView.animateWithDuration(0.25, animations: { () -> Void in
            self.ringImageView.alpha = 1.0
            self.activityIndicator.alpha = 0.0
        })
    }
    
    /// MARK: Animations
    
    private func startAnimatingRing() {
        self.ringImageView.layer.removeAllAnimations()
        
        var scaleAnimation = CABasicAnimation(keyPath: "transform.scale")
        scaleAnimation.duration = 0.4;
        scaleAnimation.repeatCount = 2048;
        scaleAnimation.autoreverses = true;
        scaleAnimation.fromValue = NSNumber(float: 1.0)
        scaleAnimation.toValue = NSNumber(float: 0.8)
        
        self.ringImageView.layer.addAnimation(scaleAnimation, forKey: "scale")
    }
    
    private func stopAnimatingRing() {
        self.ringImageView.layer.removeAllAnimations()
    }

    /// MARK: Actions
    
    @IBAction func didTapOpenDoorButton(sender: UIButton) {
        openDoor()
    }
    
    /// MARK: Door handling
    
    private func openDoor() {
        if self.connectionController.state == .Open {
            self.activityIndicator.startAnimating()
            UIView.animateWithDuration(0.25, animations: { () -> Void in
                self.ringImageView.alpha = 0.5
                self.activityIndicator.alpha = 1.0
            })
            self.openDoorButton.enabled = false
            self.connectionController.openDoor()
        }
    }
    
}

