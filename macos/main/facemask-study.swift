import SwiftUI

struct SnrSwitch: View {
    @ObservedObject var minusTenDBStartingSnr: ObservableBool

    init(minusTenDBStartingSnr: ObservableBool) {
        self.minusTenDBStartingSnr = minusTenDBStartingSnr
    }

    var body: some View {
        Toggle("-10 dB SNR", isOn: $minusTenDBStartingSnr.value)
    }
}

class AppDelegate: NSObject, NSApplicationDelegate {
    var window: NSWindow!
    let sessionUI: SwiftSessionUI
    let minusTenDBStartingSnr = ObservableBool()
    let testSetupUI: SwiftFacemaskStudyTestSetupUI

    override init() {
        sessionUI = SwiftSessionUI()
        testSetupUI = SwiftFacemaskStudyTestSetupUI {
            [minusTenDBStartingSnr] in minusTenDBStartingSnr.value ? "-10" : "0"
        }
        AvSpeechInNoiseMain.facemaskStudy(SwiftTestSetupUIFactory(testSetupUI: testSetupUI), with: sessionUI, with: sessionUI.testUI, with: sessionUI.freeResponseUI, with: sessionUI.syllablesUI, with: sessionUI.chooseKeywordsUI, with: sessionUI.correctKeywordsUI, with: sessionUI.passFailUI)
    }

    func applicationDidFinishLaunching(_: Notification) {
        let userDefaults = UserDefaults()
        sessionUI.audioDevice_.string = userDefaults.string(forKey: "AudioDevice") ?? ""

        let contentView = SwiftSessionView(ui: sessionUI) {
            SwiftFacemaskStudyTestSetupView(ui: self.testSetupUI) {
                SnrSwitch(minusTenDBStartingSnr: self.minusTenDBStartingSnr)
            }
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
        let delegate = AppDelegate()
        let menu = AppMenu()
        NSApplication.shared.delegate = delegate
        NSApplication.shared.mainMenu = menu
        _ = NSApplicationMain(CommandLine.argc, CommandLine.unsafeArgv)
    }
}
