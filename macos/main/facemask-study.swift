import SwiftUI

class AppDelegate: NSObject, NSApplicationDelegate {
    var window: NSWindow!
    let sessionUI: SwiftSessionUI
    let testSetupUI = SwiftFacemaskStudyTestSetupUI()
    
    override init() {
        sessionUI = SwiftSessionUI()
        AvSpeechInNoiseMain.facemaskStudy(SwiftTestSetupUIFactory(testSetupUI: testSetupUI), with: sessionUI, with: sessionUI.testUI, with: sessionUI.freeResponseUI, with: sessionUI.syllablesUI, with: sessionUI.chooseKeywordsUI, with: sessionUI.correctKeywordsUI, with: sessionUI.passFailUI)
    }


    func applicationDidFinishLaunching(_ aNotification: Notification) {
        let userDefaults = UserDefaults()
        sessionUI.audioDevice_.string = userDefaults.string(forKey: "AudioDevice") ?? ""
        
        let contentView = SwiftSessionView(ui: sessionUI, showingTestSetup: testSetupUI.showing) {
            SettingsView(ui: self.sessionUI)
            SwiftFacemaskStudyTestSetupView(ui: self.testSetupUI)
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
