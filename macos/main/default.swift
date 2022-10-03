import SwiftUI

class AppDelegate: NSObject, NSApplicationDelegate {
    var window: NSWindow!
    let testSettingsPathControl = NSPathControl()
    let sessionUI = SwiftSessionUI()
    let testSetupUI: SwiftTestSetupUI

    override init() {
        testSetupUI = SwiftTestSetupUI(testSettingsPathControl: testSettingsPathControl)
    }

    func applicationDidFinishLaunching(_: Notification) {
        AvSpeechInNoiseMain.default(SwiftTestSetupUIFactory(testSetupUI: testSetupUI), with: sessionUI, with: sessionUI.testUI, with: sessionUI.freeResponseUI, with: sessionUI.syllablesUI, with: sessionUI.chooseKeywordsUI, with: sessionUI.correctKeywordsUI, with: sessionUI.passFailUI)

        let userDefaults = UserDefaults()
        sessionUI.audioDevice_.string = userDefaults.string(forKey: "AudioDevice") ?? ""
        sessionUI.subjectScreen_.string = userDefaults.string(forKey: "SubjectScreen") ?? ""
        testSetupUI.subjectId_.string = userDefaults.string(forKey: "SubjectID") ?? ""
        testSetupUI.testerId_.string = userDefaults.string(forKey: "TesterID") ?? ""
        testSetupUI.session_.string = userDefaults.string(forKey: "Session") ?? ""
        testSetupUI.startingSnr_.string = userDefaults.string(forKey: "StartingSNR") ?? ""
        testSetupUI.transducer_.string = userDefaults.string(forKey: "Transducer") ?? ""
        testSetupUI.testSettingsPathControl.url = URL(fileURLWithPath: userDefaults.string(forKey: "TestSettingsFilePath") ?? "")
        testSetupUI.rmeSetting_.string = userDefaults.string(forKey: "RMESetting") ?? ""

        let contentView = SwiftSessionView(ui: sessionUI) {
            SwiftTestSetupView(ui: self.testSetupUI, testSettingsPathControl: self.testSettingsPathControl)
        }

        window = NSWindow(
            contentRect: NSRect(x: 0, y: 0, width: 480, height: 300),
            styleMask: [.titled, .closable, .miniaturizable, .resizable, .fullSizeContentView],
            backing: .buffered, defer: false
        )
        window.isReleasedWhenClosed = false
        window.center()
        window.setFrameAutosaveName("Main Window")
        window.contentView = NSHostingView(rootView: contentView)
        window.makeKeyAndOrderFront(nil)
    }

    func applicationWillTerminate(_: Notification) {
        let userDefaults = UserDefaults()
        userDefaults.set(sessionUI.audioDevice_.string, forKey: "AudioDevice")
        userDefaults.set(sessionUI.subjectScreen_.string, forKey: "SubjectScreen")
        userDefaults.set(testSetupUI.subjectId_.string, forKey: "SubjectID")
        userDefaults.set(testSetupUI.testerId_.string, forKey: "TesterID")
        userDefaults.set(testSetupUI.session_.string, forKey: "Session")
        userDefaults.set(testSetupUI.startingSnr_.string, forKey: "StartingSNR")
        userDefaults.set(testSetupUI.transducer_.string, forKey: "Transducer")
        userDefaults.set(testSetupUI.testSettingsPathControl.url?.path ?? "", forKey: "TestSettingsFilePath")
        userDefaults.set(testSetupUI.rmeSetting_.string, forKey: "RMESetting")
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
enum SwiftMain {
    static func main() {
        let terminatingAlert = NSAlert()
        terminatingAlert.messageText = ""
        switch AVCaptureDevice.authorizationStatus(for: .audio) {
            case .authorized:

            case .notDetermined: // The user has not yet been asked for camera access.
                AVCaptureDevice.requestAccess(for: .audio) { granted in
                    if granted {
                    }
                    else {                       
                        terminatingAlert.informativeText = "User does not grant microphone access. Terminating."
                        terminatingAlert.runModal()
                        NSApp.terminate(nil)
                    }
                }

            case .denied: // The user has previously denied access.
                terminatingAlert.informativeText = "User has denied microphone access. Check system privacy settings. Terminating."
                terminatingAlert.runModal()
                return

            case .restricted: // The user can't grant access due to restrictions.
                terminatingAlert.informativeText = "User is restricted from granting microphone access. Terminating."
                terminatingAlert.runModal()
                return
        }
        let delegate = AppDelegate()
        let menu = AppMenu()
        NSApplication.shared.delegate = delegate
        NSApplication.shared.mainMenu = menu
        _ = NSApplicationMain(CommandLine.argc, CommandLine.unsafeArgv)
    }
}
