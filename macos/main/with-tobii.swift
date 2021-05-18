import SwiftUI

// https://stackoverflow.com/a/66200850
struct AboutView: View {
    var body: some View {
        Image("tobii-pro-logo")
        Text("This application is powered by Tobii Pro")
    }
}

class AppDelegate: NSObject, NSApplicationDelegate {
    private var aboutBoxWindowController: NSWindowController?

    func showAboutPanel() {
        if aboutBoxWindowController == nil {
            let styleMask: NSWindow.StyleMask = [.closable, .miniaturizable,/* .resizable,*/ .titled]
            let window = NSWindow()
            window.styleMask = styleMask
            window.title = "About AV Speech in Noise"
            window.contentView = NSHostingView(rootView: AboutView())
            aboutBoxWindowController = NSWindowController(window: window)
        }

        aboutBoxWindowController?.showWindow(aboutBoxWindowController?.window)
    }
}

@main
struct SwiftCPPApp: App {
    @NSApplicationDelegateAdaptor(AppDelegate.self) var appDelegate
    let testSettingsPathControl = NSPathControl()
    let sessionUI: SwiftSessionUI
    let testSetupUI: SwiftTestSetupUI
    
    init() {
        testSetupUI = SwiftTestSetupUI(testSettingsPathControl: testSettingsPathControl)
        sessionUI = SwiftSessionUI()
        AvSpeechInNoiseMain.withTobiiPro(SwiftTestSetupUIFactory(testSetupUI: testSetupUI), with: sessionUI, with: sessionUI.testUI, with: sessionUI.freeResponseUI, with: sessionUI.syllablesUI, with: sessionUI.chooseKeywordsUI, with: sessionUI.correctKeywordsUI, with: sessionUI.passFailUI)
    }
    
    var body: some Scene {
        WindowGroup {
            SwiftSessionView(ui: sessionUI, showingTestSetup: testSetupUI.showing) {
                SwiftTestSetupView(ui: testSetupUI, testSettingsPathControl:testSettingsPathControl)
            }
        }
        .commands {
            CommandGroup(replacing: CommandGroupPlacement.appInfo) {
                Button(action: {
                    appDelegate.showAboutPanel()
                }) {
                    Text("About AV Speech in Noise")
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
