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
            let styleMask: NSWindow.StyleMask = [.closable, .miniaturizable, .titled]
            let window = NSWindow()
            window.styleMask = styleMask
            window.title = "About Tobii Pro"
            window.contentView = NSHostingView(rootView: AboutView())
            aboutBoxWindowController = NSWindowController(window: window)
        }

        aboutBoxWindowController?.showWindow(aboutBoxWindowController?.window)
    }
}

class SwiftEyeTrackerRunMenu: NSObject, EyeTrackerRunMenu {
    var observer: EyeTrackerMenuObserver?

    func attach(_ observer: EyeTrackerMenuObserver!) {
        self.observer = observer
    }
}

struct CalibrationValidationTesterSwiftView: View {
    @ObservedObject var showing: ObservableBool
    @ObservedObject var leftEyeAccuracyDegrees: ObservableString
    @ObservedObject var rightEyeAccuracyDegrees: ObservableString
    @ObservedObject var leftEyePrecisionDegrees: ObservableString
    @ObservedObject var rightEyePrecisionDegrees: ObservableString
    @ObservedObject var observableObserver: EyeTrackerCalibrationValidationControlObservableObserver

    init(ui: EyeTrackerCalibrationValidationTesterUI) {
        showing = ui.showing
        leftEyePrecisionDegrees = ui.leftEyePrecisionDegrees
        leftEyeAccuracyDegrees = ui.leftEyeAccuracyDegrees
        rightEyePrecisionDegrees = ui.rightEyePrecisionDegrees
        rightEyeAccuracyDegrees = ui.rightEyeAccuracyDegrees
        observableObserver = ui.observableObserver
    }

    var body: some View {
        if showing.value {
            HStack {
                List {
                    Form {
                        Section(header: Text("accuracy (degrees)")) {
                            TextField("left", text: $leftEyeAccuracyDegrees.string).disabled(true)
                            TextField("right", text: $rightEyeAccuracyDegrees.string).disabled(true)
                        }
                    }
                }
                List {
                    Form {
                        Section(header: Text("precision (degrees)")) {
                            TextField("left", text: $leftEyePrecisionDegrees.string).disabled(true)
                            TextField("right", text: $rightEyePrecisionDegrees.string).disabled(true)
                        }
                    }
                }
                Button("Close") {
                    observableObserver.observer?.notifyThatCloseButtonHasBeenClicked()
                }
            }.frame(height: 200)
        }
    }
}

class EyeTrackerCalibrationValidationControlObservableObserver: ObservableObject {
    @Published var observer: AvSpeechInNoiseEyeTrackerCalibrationValidationControlObserver? = nil
}

class EyeTrackerCalibrationValidationTesterUI: NSObject, AvSpeechInNoiseEyeTrackerCalibrationValidationTesterUI {
    let observableObserver = EyeTrackerCalibrationValidationControlObservableObserver()
    let showing = ObservableBool()
    let leftEyeAccuracyDegrees = ObservableString()
    let rightEyeAccuracyDegrees = ObservableString()
    let leftEyePrecisionDegrees = ObservableString()
    let rightEyePrecisionDegrees = ObservableString()

    func attach(_ observer: AvSpeechInNoiseEyeTrackerCalibrationValidationControlObserver!) {
        observableObserver.observer = observer
    }

    func setLeftEyeAccuracyDegrees(_ degrees: String!) {
        leftEyeAccuracyDegrees.string = degrees
    }

    func setLeftEyePrecisionDegrees(_ degrees: String!) {
        leftEyePrecisionDegrees.string = degrees
    }

    func setRightEyeAccuracyDegrees(_ degrees: String!) {
        rightEyeAccuracyDegrees.string = degrees
    }

    func setRightEyePrecisionDegrees(_ degrees: String!) {
        rightEyePrecisionDegrees.string = degrees
    }

    func show() {
        showing.value = true
    }

    func hide() {
        showing.value = false
    }
}

class AppDelegate: NSObject, NSApplicationDelegate {
    var window: NSWindow!
    let testSettingsPathControl = NSPathControl()
    let sessionUI: SwiftSessionUI
    let testSetupUI: SwiftTestSetupUI
    let eyeTrackerRunMenu: SwiftEyeTrackerRunMenu
    let eyeTrackerCalibrationValidationTesterUI: EyeTrackerCalibrationValidationTesterUI

    init(eyeTrackerRunMenu: SwiftEyeTrackerRunMenu, eyeTrackerCalibrationValidationTesterUI: EyeTrackerCalibrationValidationTesterUI) {
        testSetupUI = SwiftTestSetupUI(testSettingsPathControl: testSettingsPathControl)
        sessionUI = SwiftSessionUI()
        self.eyeTrackerRunMenu = eyeTrackerRunMenu
        self.eyeTrackerCalibrationValidationTesterUI = eyeTrackerCalibrationValidationTesterUI
    }

    func applicationDidFinishLaunching(_: Notification) {
        let alertWindow = NSWindow(contentRect: NSRect(
            x: 0,
            y: 0,
            width: 0,
            height: 0
        ), styleMask: .borderless, backing: .buffered, defer: false)
        
        let userDefaults = UserDefaults()
        let userDefaultSubjectScreen = userDefaults.string(forKey: "SubjectScreen") ?? ""
        if let screen = NSScreen.screens.first(where: {
            (screen) -> Bool in  return screen.localizedName == userDefaultSubjectScreen
        }) {
            alertWindow.setFrame(screen.frame, display: true);
        } else if let screen = NSScreen.screens.last {
            alertWindow.setFrame(screen.frame, display: true);
        }
        let alert = NSAlert()
        alert.messageText = ""
        alert.informativeText = "This software will store your eye tracking data.\n\nWe do so only for the purpose of the current study (11-24-XP). We never sell, distribute, or make profit on the collected data. Only staff and research personnel on the existing IRB will have access to the data. Any data used for publication or collaborative and/or learning purposes will be deidentified.\n\nThere is no direct benefit to you for doing this study. What we learn from this study may help doctors treat children who have a hard time hearing when it is noisy."
        alert.addButton(withTitle: "Yes, I accept")
        alert.addButton(withTitle: "No, I do not accept")
        alert.beginSheetModal(for: alertWindow, completionHandler: { (response: NSApplication.ModalResponse) in
            alertWindow.orderOut(nil)
            NSApp.stopModal(withCode: response)
        })
        if NSApp.runModal(for: alertWindow) == NSApplication.ModalResponse.alertSecondButtonReturn {
            let terminatingAlert = NSAlert()
            terminatingAlert.messageText = ""
            terminatingAlert.informativeText = "User does not accept eye tracking terms. Terminating."
            terminatingAlert.runModal()
            NSApp.terminate(nil)
        }
        NSLog("Initializing application...")
        AvSpeechInNoiseMain.withTobiiPro(SwiftTestSetupUIFactory(testSetupUI: testSetupUI), with: sessionUI, with: sessionUI.testUI, with: sessionUI.freeResponseUI, with: sessionUI.syllablesUI, with: sessionUI.chooseKeywordsUI, with: sessionUI.correctKeywordsUI, with: sessionUI.passFailUI, withEyeTrackerMenu: eyeTrackerRunMenu, with: eyeTrackerCalibrationValidationTesterUI)
        
        NSLog("Restoring user defaults...")
        sessionUI.audioDevice_.string = userDefaults.string(forKey: "AudioDevice") ?? ""
        sessionUI.subjectScreen_.string = userDefaultSubjectScreen
        testSetupUI.subjectId_.string = userDefaults.string(forKey: "SubjectID") ?? ""
        testSetupUI.testerId_.string = userDefaults.string(forKey: "TesterID") ?? ""
        testSetupUI.session_.string = userDefaults.string(forKey: "Session") ?? ""
        testSetupUI.startingSnr_.string = userDefaults.string(forKey: "StartingSNR") ?? ""
        testSetupUI.transducer_.string = userDefaults.string(forKey: "Transducer") ?? ""
        testSetupUI.testSettingsPathControl.url = URL(fileURLWithPath: userDefaults.string(forKey: "TestSettingsFilePath") ?? "")
        testSetupUI.rmeSetting_.string = userDefaults.string(forKey: "RMESetting") ?? ""
        
        NSLog("Creating session view...")
        let contentView = SwiftSessionView(ui: sessionUI) {
            SwiftTestSetupView(ui: self.testSetupUI, testSettingsPathControl: self.testSettingsPathControl)
            CalibrationValidationTesterSwiftView(ui: self.eyeTrackerCalibrationValidationTesterUI)
        }
        
        NSLog("Creating main window...")
        window = NSWindow(
            contentRect: NSRect(x: 0, y: 0, width: 480, height: 300),
            styleMask: [.titled, .closable, .miniaturizable, .resizable, .fullSizeContentView],
            backing: .buffered, defer: false
        )
        window.isReleasedWhenClosed = false
        
        NSLog("Centering main window...")
        if let screen = NSScreen.screens.first {
            window.setFrameOrigin(NSMakePoint((screen.frame.width - 480) / 2, (screen.frame.height - 300) / 2))
        }
        window.contentView = NSHostingView(rootView: contentView)
        window.makeKeyAndOrderFront(nil)
        NSLog("Finished opening application.")
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

class AppMenuReceiver: NSObject {
    let aboutTobiiPro = AboutTobiiPro()
    let eyeTrackerRunMenu: SwiftEyeTrackerRunMenu
    let eyeTrackerCalibrationValidationTesterUI: EyeTrackerCalibrationValidationTesterUI

    init(eyeTrackerRunMenu: SwiftEyeTrackerRunMenu, eyeTrackerCalibrationValidationTesterUI: EyeTrackerCalibrationValidationTesterUI) {
        self.eyeTrackerRunMenu = eyeTrackerRunMenu
        self.eyeTrackerCalibrationValidationTesterUI = eyeTrackerCalibrationValidationTesterUI
    }

    @objc
    func notifyThatAboutTobiiProHasBeenClicked() {
        aboutTobiiPro.showAboutPanel()
    }

    @objc
    func notifyThatRunEyeTrackerCalibrationHasBeenClicked() {
        eyeTrackerRunMenu.observer?.notifyThatRunCalibrationHasBeenClicked()
    }

    @objc
    func notifyThatRunEyeTrackerCalibrationValidationHasBeenClicked() {
        eyeTrackerCalibrationValidationTesterUI.observableObserver.observer?.notifyThatMenuHasBeenSelected()
    }
}

@main
enum SwiftMain {
    static func main() {
        let eyeTrackerRunMenu = SwiftEyeTrackerRunMenu()
        let eyeTrackerCalibrationValidationTesterUI = EyeTrackerCalibrationValidationTesterUI()
        let appMenuReceiver = AppMenuReceiver(eyeTrackerRunMenu: eyeTrackerRunMenu, eyeTrackerCalibrationValidationTesterUI: eyeTrackerCalibrationValidationTesterUI)
        let applicationName = ProcessInfo.processInfo.processName
        let menu = NSMenu()
        let menuItemOne = NSMenuItem()
        menuItemOne.submenu = NSMenu(title: "menuItemOne")
        menuItemOne.submenu?.items = [NSMenuItem(title: "Quit \(applicationName)", action: #selector(NSApplication.terminate(_:)), keyEquivalent: "q")]
        let legalMenu = NSMenuItem()
        legalMenu.submenu = NSMenu(title: "Legal")
        let aboutTobiiProMenuItem = NSMenuItem(title: "About Tobii Pro", action: #selector(AppMenuReceiver.notifyThatAboutTobiiProHasBeenClicked), keyEquivalent: "")
        aboutTobiiProMenuItem.target = appMenuReceiver
        legalMenu.submenu?.items = [aboutTobiiProMenuItem]
        let runMenu = NSMenuItem()
        runMenu.submenu = NSMenu(title: "Run")
        let runEyeTrackerCalibration = NSMenuItem(title: "Eye Tracker Calibration", action: #selector(AppMenuReceiver.notifyThatRunEyeTrackerCalibrationHasBeenClicked), keyEquivalent: "")
        runEyeTrackerCalibration.target = appMenuReceiver
        let runEyeTrackerCalibrationValidation = NSMenuItem(title: "Eye Tracker Calibration Validation", action: #selector(AppMenuReceiver.notifyThatRunEyeTrackerCalibrationValidationHasBeenClicked), keyEquivalent: "")
        runEyeTrackerCalibrationValidation.target = appMenuReceiver
        runMenu.submenu?.items = [runEyeTrackerCalibration, runEyeTrackerCalibrationValidation]
        menu.items = [menuItemOne, runMenu, legalMenu]
        let delegate = AppDelegate(eyeTrackerRunMenu: eyeTrackerRunMenu, eyeTrackerCalibrationValidationTesterUI: eyeTrackerCalibrationValidationTesterUI)
        NSApplication.shared.delegate = delegate
        NSApplication.shared.mainMenu = menu
        _ = NSApplicationMain(CommandLine.argc, CommandLine.unsafeArgv)
    }
}
