//
//  ViewController.swift
//  DOAR
//
//  Created by Sascha Schwabbauer on 22/12/14.
//  Copyright (c) 2014 evolved.io. All rights reserved.
//

import UIKit
import AudioToolbox
import DOARKitiOS

class ViewController: UIViewController {
    enum OpenDoorState {
        case Idle
        case Opening
    }
    
    @IBOutlet var statusLabel: UILabel!
    @IBOutlet var openDoorButton: UIButton!
    @IBOutlet var activityIndicator: UIActivityIndicatorView!
    @IBOutlet var ringImageView: UIImageView!
    var tapGestureRecognizer: UITapGestureRecognizer!
    
    var doorSound: SystemSoundID = 0
    var connectionController = ConnectionController()
    var connectionTimer: NSTimer?
    var state = OpenDoorState.Idle
    
    var connected: Bool = false {
        didSet {
            openDoorButton.enabled = connected
            tapGestureRecognizer.enabled = connected
            statusLabel.text = connected ? NSLocalizedString("label.state.connected", comment: "") : NSLocalizedString("label.state.disconnected", comment: "")
            
            if (!connected) {
                UIApplication.sharedApplication().cancelAllLocalNotifications()
                self.connectionTimer = NSTimer.scheduledTimerWithTimeInterval(10, target: self, selector: "connectToServer", userInfo: nil, repeats: false)
                showActivityIndicator(true)
            } else if let timer = self.connectionTimer? {
                self.connectionTimer!.invalidate()
                self.connectionTimer = nil
                self.hideActivityIndicator(true)
            }
        }
    }
    
    func connectToServer() {
        self.connectionController.connect()
    }
    
    // MARK: - UIViewController
    
    override func viewDidLoad() {
        super.viewDidLoad()
        
        self.tapGestureRecognizer = UITapGestureRecognizer(target: self, action: "didTapOpenDoorButton:")
        self.view.addGestureRecognizer(self.tapGestureRecognizer)
        
        if let soundURL = NSBundle.mainBundle().URLForResource("doorbell", withExtension: "m4a") {
            AudioServicesCreateSystemSoundID(soundURL, &doorSound)
        }
        
        self.registerForNotifications()
        self.connectToServer()
    }
    
    deinit {
        self.unregisterFromNotifications()
    }
    
    // MARK: - Notifications
    
    private func registerForNotifications() {
        NSNotificationCenter.defaultCenter().addObserver(self, selector: "connectionStateDidChange", name: AppConfiguration.Notifications.ConnectionStateDidChangeNotification, object: nil)
        NSNotificationCenter.defaultCenter().addObserver(self, selector: "doorRingReceived", name: AppConfiguration.Notifications.ConnectionDidReceiveDoorRingNotification, object: nil)
        NSNotificationCenter.defaultCenter().addObserver(self, selector: "willOpenDoorReceived", name: AppConfiguration.Notifications.ConnectionDidReceiveWillOpenDoorNotification, object: nil)
        NSNotificationCenter.defaultCenter().addObserver(self, selector: "didOpenDoorReceived", name: AppConfiguration.Notifications.ConnectionDidReceiveDidOpenDoorNotification, object: nil)
    }
    
    func unregisterFromNotifications() {
        let notificationCenter = NSNotificationCenter.defaultCenter()
        notificationCenter.removeObserver(self, name: AppConfiguration.Notifications.ConnectionStateDidChangeNotification, object: nil)
        notificationCenter.removeObserver(self, name: AppConfiguration.Notifications.ConnectionDidReceiveDoorRingNotification, object: nil)
        notificationCenter.removeObserver(self, name: AppConfiguration.Notifications.ConnectionDidReceiveDidOpenDoorNotification, object: nil)
    }
    
    func connectionStateDidChange() {
        self.connected = self.connectionController.state == .Open
    }
    
    func doorRingReceived() {
        if self.state == .Opening {
            return
        }
        
        UIApplication.sharedApplication().cancelAllLocalNotifications()
        let notification = UILocalNotification()
        let title = NSLocalizedString("notification.door-ring.title", comment: "")
        let subtitle = NSLocalizedString("notification.door-ring.subtitle", comment: "")
        notification.alertBody = "\(title) \(subtitle)"
        notification.alertAction = NSLocalizedString("notification.door-ring.action-button-title", comment: "")
        notification.soundName = "doorbell.m4a"
        notification.category = "doorRing"
        
        UIApplication.sharedApplication().presentLocalNotificationNow(notification)
        
        AudioServicesPlaySystemSound(doorSound)
        AudioServicesPlaySystemSound(SystemSoundID(kSystemSoundID_Vibrate))
        self.startAnimatingRing()
    }
    
    func willOpenDoorReceived() {
        UIApplication.sharedApplication().cancelAllLocalNotifications()
        self.state = .Opening
        self.openDoorButton.enabled = false
        self.tapGestureRecognizer.enabled = false
        self.showActivityIndicator(true)
    }
    
    func didOpenDoorReceived() {
        self.state = .Idle
        self.openDoorButton.enabled = true
        self.tapGestureRecognizer.enabled = true
        self.hideActivityIndicator(true)
    }
    
    // MARK: - Actions
    
    @IBAction func didTapOpenDoorButton(sender: AnyObject?) {
        if self.connectionController.state == .Open {
            self.showActivityIndicator(true)
            self.openDoorButton.enabled = false
            self.tapGestureRecognizer.enabled = false
            self.connectionController.openDoor()
        }
    }
    
    // MARK: - Animations
    
    private func startAnimatingRing() {
        self.ringImageView.layer.removeAllAnimations()
        
        var scaleAnimation = CABasicAnimation(keyPath: "transform.scale")
        scaleAnimation.duration = 0.4
        scaleAnimation.repeatCount = 3
        scaleAnimation.autoreverses = true
        scaleAnimation.fromValue = NSNumber(float: 1.0)
        scaleAnimation.toValue = NSNumber(float: 0.8)
        
        self.ringImageView.layer.addAnimation(scaleAnimation, forKey: "scale")
    }
    
    private func stopAnimatingRing() {
        self.ringImageView.layer.removeAllAnimations()
    }
    
    private func showActivityIndicator(animated: Bool) {
        self.activityIndicator.startAnimating()
        UIView.animateWithDuration(animated ? 0.25 : 0,
            animations: { () -> Void in
                self.ringImageView.alpha = 0.5
                self.activityIndicator.alpha = 1.0
        })
    }
    
    private func hideActivityIndicator(animated: Bool) {
        UIView.animateWithDuration(animated ? 0.25 : 0,
            animations: { () -> Void in
                self.ringImageView.alpha = 1.0
                self.activityIndicator.alpha = 0.0
            }) { _ in
                self.activityIndicator.stopAnimating()
        }
    }
}

