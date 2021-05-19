import SwiftUI

// https://stackoverflow.com/a/66200850
struct AboutView: View {
    var body: some View {
        Image("tobii-pro-logo")
        Text("This application is powered by Tobii Pro")
    }
}

class AboutTobiiPro: NSObject {
    private var aboutBoxWindowController: NSWindowController?

    func showAboutPanel() {
        if aboutBoxWindowController == nil {
            let styleMask: NSWindow.StyleMask = [.closable, .miniaturizable,/* .resizable,*/ .titled]
            let window = NSWindow()
            window.styleMask = styleMask
            window.title = "About Tobii Pro"
            window.contentView = NSHostingView(rootView: AboutView())
            aboutBoxWindowController = NSWindowController(window: window)
        }

        aboutBoxWindowController?.showWindow(aboutBoxWindowController?.window)
    }
}

class EyeTrackerMenuObserverObservable : ObservableObject {
    @Published var observer : EyeTrackerMenuObserver? = nil
}

class SwiftEyeTrackerRunMenu : NSObject, EyeTrackerRunMenu {
    let observableObserver = EyeTrackerMenuObserverObservable()
    
    func attach(_ observer: EyeTrackerMenuObserver!) {
        observableObserver.observer = observer
    }
}

class AppDelegate: NSObject, NSApplicationDelegate {
    var window: NSWindow!
    let aboutTobiiPro = AboutTobiiPro()
    @ObservedObject var eyeTrackerRunMenuObservable: EyeTrackerMenuObserverObservable
    let testSettingsPathControl = NSPathControl()
    let sessionUI: SwiftSessionUI
    let testSetupUI: SwiftTestSetupUI
    let eyeTrackerRunMenu = SwiftEyeTrackerRunMenu()
    
    override init() {
        testSetupUI = SwiftTestSetupUI(testSettingsPathControl: testSettingsPathControl)
        sessionUI = SwiftSessionUI()
        eyeTrackerRunMenuObservable = eyeTrackerRunMenu.observableObserver
    }


    func applicationDidFinishLaunching(_ aNotification: Notification) {
        let subjectScreen = NSScreen.screens.last
        let subjectScreenOrigin = subjectScreen?.frame.origin ?? NSPoint()
        let subjectScreenSize = subjectScreen?.frame.size ?? NSSize()
        let alertWindow = NSWindow.init(contentRect: NSRect(
                                            x: subjectScreenOrigin.x,
                                            y: subjectScreenOrigin.y,
                                            width: subjectScreenSize.width,
                                            height: subjectScreenSize.height), styleMask: .borderless, backing: .buffered, defer: false)
        let alert = NSAlert()
        alert.messageText = ""
        alert.informativeText = "This software will store your eye tracking data.\n\nWe do so only for the purpose of the current study (17-13-XP). We never sell, distribute, or make profit on the collected data. Only staff and research personnel on the existing IRB will have access to the data. Any data used for publication or collaborative and/or learning purposes will be deidentified.\n\nThere is no direct benefit to you for doing this study. What we learn from this study may help doctors treat children who have a hard time hearing when it is noisy."
        alert.addButton(withTitle: "No, I do not accept")
        alert.addButton(withTitle: "Yes, I accept")
        alertWindow.makeKeyAndOrderFront(nil)
        alert.beginSheetModal(for: alertWindow, completionHandler: {(response: NSApplication.ModalResponse) in
            alertWindow.orderOut(nil)
            NSApp.stopModal(withCode: response)
        })
        if (NSApp.runModal(for: alertWindow) == NSApplication.ModalResponse.alertFirstButtonReturn) {
            let terminatingAlert = NSAlert()
            terminatingAlert.messageText = ""
            terminatingAlert.informativeText = "User does not accept eye tracking terms. Terminating."
            terminatingAlert.runModal()
            NSApp.terminate(nil)
        }
        AvSpeechInNoiseMain.withTobiiPro(SwiftTestSetupUIFactory(testSetupUI: testSetupUI), with: sessionUI, with: sessionUI.testUI, with: sessionUI.freeResponseUI, with: sessionUI.syllablesUI, with: sessionUI.chooseKeywordsUI, with: sessionUI.correctKeywordsUI, with: sessionUI.passFailUI, withEyeTrackerMenu: eyeTrackerRunMenu)
        
        let userDefaults = UserDefaults()
        sessionUI.audioDevice_.string = userDefaults.string(forKey: "AudioDevice") ?? ""
        
        let contentView = SwiftSessionView(ui: sessionUI, showingTestSetup: testSetupUI.showing) {
            SettingsView(ui: self.sessionUI)
            SwiftTestSetupView(ui: self.testSetupUI, testSettingsPathControl:self.testSettingsPathControl)
            Button("Eye Tracker Calibration") {
                self.eyeTrackerRunMenuObservable.observer?.notifyThatRunCalibrationHasBeenClicked()
            }
            Button("About Tobii Pro") {
                self.aboutTobiiPro.showAboutPanel()
            }
        }

        window = NSWindow(
            contentRect: NSRect(x: 0, y: 0, width: 480, height: 300),
            styleMask: [.titled, .closable, .miniaturizable, .resizable, .fullSizeContentView],
            backing: .buffered, defer: false)
        window.isReleasedWhenClosed = false
        if let screen = NSScreen.screens.first {
            window.setFrameOrigin(NSMakePoint((screen.frame.width - 480)/2, (screen.frame.height - 300)/2))
        }
        window.contentView = NSHostingView(rootView: contentView)
        window.makeKeyAndOrderFront(nil)
    }

    func applicationWillTerminate(_ aNotification: Notification) {
        let userDefaults = UserDefaults()
        userDefaults.set(sessionUI.audioDevice_.string, forKey: "AudioDevice")
    }
}

class AppMenu: NSMenu {
    private lazy var applicationName = ProcessInfo.processInfo.processName
    
    override init(title: String) {
        super.init(title: title)
        
        let menuItemOne = NSMenuItem()
        menuItemOne.submenu = NSMenu(title: "menuItemOne")
        menuItemOne.submenu?.items = [NSMenuItem(title: "Quit \(applicationName)", action: #selector(NSApplication.terminate(_:)), keyEquivalent: "q")]
        items = [menuItemOne]
    }
    
    required init(coder: NSCoder) {
        super.init(coder: coder)
    }
}

@main
class SwiftMain {
    static func main() {
        let delegate = AppDelegate()
        let menu = AppMenu()
        NSApplication.shared.delegate = delegate
        NSApplication.shared.mainMenu = menu
        _ = NSApplicationMain(CommandLine.argc, CommandLine.unsafeArgv)
    }
}
