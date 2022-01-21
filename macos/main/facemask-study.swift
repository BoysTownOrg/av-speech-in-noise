import SwiftUI

func conditionShortName(stem: String, testSettingsForShortName: inout [String: String]) -> String {
    let name = stem
    testSettingsForShortName[name] = AvSpeechInNoiseUtility.resourcePath(stem, withExtension: "txt")
    return name
}

struct SwiftFacemaskStudyTestSetupView<Content: View>: View {
    @ObservedObject var subjectID_: ObservableString
    @ObservedObject var testSettingsShortName: ObservableString
    @ObservedObject var observableObserver: TestSetupUIObserverObservable
    @ObservedObject var testSettingsShortNames: ObservableStringCollection
    @ObservedObject var showing: ObservableBool
    let snrView: Content

    init(ui: SwiftFacemaskStudyTestSetupUI, @ViewBuilder snrView: @escaping () -> Content) {
        subjectID_ = ui.subjectID_
        testSettingsShortName = ui.testSettingsShortName
        observableObserver = ui.observableObserver
        testSettingsShortNames = ui.testSettingsShortNames
        showing = ui.showing
        self.snrView = snrView()
    }

    var body: some View {
        if showing.value {
            VStack {
                HStack {
                    Image("btnrh").resizable().aspectRatio(contentMode: .fit).frame(width: 318, height: 141, alignment: .topLeading).background(Color(.white))
                    Text("Facemask Study").font(.system(size: 48)).fixedSize(horizontal: true, vertical: false)
                }.padding()
                Form {
                    TextField("Subject ID:", text: $subjectID_.string).font(.largeTitle).foregroundColor(.yellow)
                    Picker("Condition:", selection: $testSettingsShortName.string) {
                        ForEach(testSettingsShortNames.items) {
                            Text($0.string)
                        }
                    }.font(.largeTitle).foregroundColor(.yellow)
                    snrView
                    Button("play left speaker", action: {
                        observableObserver.observer?.notifyThatPlayLeftSpeakerCalibrationButtonHasBeenClicked()
                    })
                    Button("play right speaker", action: {
                        observableObserver.observer?.notifyThatPlayRightSpeakerCalibrationButtonHasBeenClicked()
                    })
                    Button(action: {
                        observableObserver.observer?.notifyThatConfirmButtonHasBeenClicked()
                    }) {
                        Text("START").font(.largeTitle)
                            .padding()
                            .background(RoundedRectangle(cornerRadius: 8).fill(Color(Color.RGBColorSpace.sRGB, red: 114 / 255, green: 172 / 255, blue: 77 / 255, opacity: 1)))
                            .frame(minWidth: 100)
                    }
                    .buttonStyle(PlainButtonStyle())
                }.padding()
            }.background(Color(Color.RGBColorSpace.sRGB, red: 43 / 255, green: 97 / 255, blue: 198 / 255, opacity: 1))
        }
    }
}

class SwiftFacemaskStudyTestSetupUI: NSObject, TestSetupUI {
    let subjectID_ = ObservableString()
    let testSettingsShortName = ObservableString()
    let observableObserver = TestSetupUIObserverObservable()
    let showing = ObservableBool()
    let testSettingsShortNames = ObservableStringCollection()
    var testSettingsForShortName = [String: String]()
    let snrFunctor: () -> String

    init(snrFunctor: @escaping () -> String) {
        testSettingsShortNames.items = [
            IdentifiableString(string: conditionShortName(stem: "NoMask_AO", testSettingsForShortName: &testSettingsForShortName)),
            IdentifiableString(string: conditionShortName(stem: "NoMask_AV", testSettingsForShortName: &testSettingsForShortName)),
            IdentifiableString(string: conditionShortName(stem: "CommunicatorMask_AO", testSettingsForShortName: &testSettingsForShortName)),
            IdentifiableString(string: conditionShortName(stem: "CommunicatorMask_AV", testSettingsForShortName: &testSettingsForShortName)),
        ]
        showing.value = true
        self.snrFunctor = snrFunctor
    }

    func show() {
        showing.value = true
    }

    func hide() {
        showing.value = false
    }

    func testerId() -> String! {
        return ""
    }

    func subjectId() -> String! {
        return subjectID_.string
    }

    func session() -> String! {
        return ""
    }

    func testSettingsFile() -> String! {
        return testSettingsForShortName[testSettingsShortName.string]
    }

    func startingSnr() -> String! {
        return snrFunctor()
    }

    func transducer() -> String! {
        return ""
    }

    func rmeSetting() -> String! {
        return ""
    }

    func populateTransducerMenu(_: [String]!) {}

    func attach(_ observer: TestSetupUIObserver!) {
        observableObserver.observer = observer
    }
}

struct SnrSwitch: View {
    @ObservedObject var minusTenDBStartingSnr: ObservableBool

    init(minusTenDBStartingSnr: ObservableBool) {
        self.minusTenDBStartingSnr = minusTenDBStartingSnr
    }

    var body: some View {
        Toggle("-10 dB SNR", isOn: $minusTenDBStartingSnr.value)
    }
}

class AppDelegate<Content: View>: NSObject, NSApplicationDelegate {
    var window: NSWindow!
    let sessionUI: SwiftSessionUI
    let minusTenDBStartingSnr = ObservableBool()
    let testSetupUI: SwiftFacemaskStudyTestSetupUI
    let snrView: () -> Content
    
    init(snrFunctor: @escaping () -> String, @ViewBuilder snrView: @escaping () -> Content) {
        sessionUI = SwiftSessionUI()
        testSetupUI = SwiftFacemaskStudyTestSetupUI(snrFunctor: snrFunctor)
        self.snrView = snrView
        AvSpeechInNoiseMain.facemaskStudy(SwiftTestSetupUIFactory(testSetupUI: testSetupUI), with: sessionUI, with: sessionUI.testUI, with: sessionUI.freeResponseUI, with: sessionUI.syllablesUI, with: sessionUI.chooseKeywordsUI, with: sessionUI.correctKeywordsUI, with: sessionUI.passFailUI)
    }

    func applicationDidFinishLaunching(_: Notification) {
        let userDefaults = UserDefaults()
        sessionUI.audioDevice_.string = userDefaults.string(forKey: "AudioDevice") ?? ""
        sessionUI.subjectScreen_.string = userDefaults.string(forKey: "SubjectScreen") ?? ""

        let contentView = SwiftSessionView(ui: sessionUI) {
            SwiftFacemaskStudyTestSetupView(ui: self.testSetupUI, snrView: self.snrView)
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
        let minusTenDBStartingSnr = ObservableBool()
        let delegate = AppDelegate (snrFunctor: {
            [minusTenDBStartingSnr] in minusTenDBStartingSnr.value ? "-10" : "0"
        }) {
            SnrSwitch(minusTenDBStartingSnr: minusTenDBStartingSnr)
        }
        let menu = AppMenu()
        NSApplication.shared.delegate = delegate
        NSApplication.shared.mainMenu = menu
        _ = NSApplicationMain(CommandLine.argc, CommandLine.unsafeArgv)
    }
}
