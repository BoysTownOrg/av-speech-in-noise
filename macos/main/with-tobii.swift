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

class EyeTrackerMenuObserverObservable : ObservableObject {
    @Published var observer : EyeTrackerMenuObserver? = nil
}

class SwiftEyeTrackerRunMenu : NSObject, EyeTrackerRunMenu {
    let observableObserver = EyeTrackerMenuObserverObservable()
    
    func attach(_ observer: EyeTrackerMenuObserver!) {
        observableObserver.observer = observer
    }
}

@main
struct SwiftCPPApp: App {
    @NSApplicationDelegateAdaptor(AppDelegate.self) var appDelegate
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
            CommandGroup(replacing: CommandGroupPlacement.appInfo) {
                Button(action: {
                    appDelegate.showAboutPanel()
                }) {
                    Text("About AV Speech in Noise")
                }
            }
            CommandMenu("Run") {
                Button("Eye Tracker Calibration") {
                    eyeTrackerRunMenuObservable.observer?.notifyThatRunCalibrationHasBeenClicked()
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
