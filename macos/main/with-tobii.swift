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
        AvSpeechInNoiseMain.withTobiiPro(SwiftTestSetupUIFactory(testSetupUI: testSetupUI), with: sessionUI, with: sessionUI.testUI, with: sessionUI.freeResponseUI, with: sessionUI.syllablesUI, with: sessionUI.chooseKeywordsUI, with: sessionUI.correctKeywordsUI, with: sessionUI.passFailUI, withEyeTrackerMenu: eyeTrackerRunMenu)
    }


    func applicationDidFinishLaunching(_ aNotification: Notification) {
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
        window.center()
        window.setFrameAutosaveName("Main Window")
        window.contentView = NSHostingView(rootView: contentView)
        window.makeKeyAndOrderFront(nil)
    }

    func applicationWillTerminate(_ aNotification: Notification) {
        let userDefaults = UserDefaults()
        userDefaults.set(sessionUI.audioDevice_.string, forKey: "AudioDevice")
    }
}

@main
class SwiftMain {
    static func main() {
        let delegate = AppDelegate()
        NSApplication.shared.delegate = delegate
        _ = NSApplicationMain(CommandLine.argc, CommandLine.unsafeArgv)
    }
}
