import Cocoa
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

/*
struct SwiftCPPApp: App {
    let aboutTobiiPro = AboutTobiiPro()
    @ObservedObject var eyeTrackerRunMenuObservable: EyeTrackerMenuObserverObservable
    let testSettingsPathControl = NSPathControl()
    let sessionUI: SwiftSessionUI
    let testSetupUI: SwiftTestSetupUI
    let eyeTrackerRunMenu = SwiftEyeTrackerRunMenu()
    
    init() {
        testSetupUI = SwiftTestSetupUI(testSettingsPathControl: testSettingsPathControl)
        sessionUI = SwiftSessionUI()
        eyeTrackerRunMenuObservable = eyeTrackerRunMenu.observableObserver
        AvSpeechInNoiseMain.withTobiiPro(SwiftTestSetupUIFactory(testSetupUI: testSetupUI), with: sessionUI, with: sessionUI.testUI, with: sessionUI.freeResponseUI, with: sessionUI.syllablesUI, with: sessionUI.chooseKeywordsUI, with: sessionUI.correctKeywordsUI, with: sessionUI.passFailUI, withEyeTrackerMenu: eyeTrackerRunMenu)
    }
    
    var body: some Scene {
        WindowGroup {
            SwiftSessionView(ui: sessionUI, showingTestSetup: testSetupUI.showing) {
                SwiftTestSetupView(ui: testSetupUI, testSettingsPathControl:testSettingsPathControl)
            }
        }
        .commands {
            CommandMenu("Run") {
                Button("Eye Tracker Calibration") {
                    eyeTrackerRunMenuObservable.observer?.notifyThatRunCalibrationHasBeenClicked()
                }
            }
            CommandMenu("Legal") {
                Button("About Tobii Pro") {
                    aboutTobiiPro.showAboutPanel()
                }
            }
        }
        #if os(macOS)
        Settings {
            SettingsView(ui: sessionUI)
        }
        #endif
    }
}
*/

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
        // Create the SwiftUI view that provides the window contents.
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

        // Create the window and set the content view.
        window = NSWindow(
            contentRect: NSRect(x: 0, y: 0, width: 480, height: 300),
            styleMask: [.titled, .closable, .miniaturizable, .resizable, .fullSizeContentView],
            backing: .buffered, defer: false)
        window.isReleasedWhenClosed = false
        window.center()
        // window.setFrameAutosaveName("Main Window")
        window.contentView = NSHostingView(rootView: contentView)
        window.makeKeyAndOrderFront(nil)
    }

    func applicationWillTerminate(_ aNotification: Notification) {
        // Insert code here to tear down your application
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
