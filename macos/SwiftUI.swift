import SwiftUI
import Foundation
import AppKit

// https://www.swiftbysundell.com/tips/inline-wrapping-of-uikit-or-appkit-views-within-swiftui/
struct Wrap<Wrapped: NSView>: NSViewRepresentable {
    typealias Updater = (Wrapped, Context) -> Void

    var makeView: () -> Wrapped
    var update: (Wrapped, Context) -> Void

    init(_ makeView: @escaping @autoclosure () -> Wrapped,
         updater update: @escaping Updater) {
        self.makeView = makeView
        self.update = update
    }

    func makeNSView(context: Context) -> Wrapped {
        makeView()
    }

    func updateNSView(_ view: Wrapped, context: Context) {
        update(view, context)
    }
}

extension Wrap {
    init(_ makeView: @escaping @autoclosure () -> Wrapped,
         updater update: @escaping (Wrapped) -> Void) {
        self.makeView = makeView
        self.update = { view, _ in update(view) }
    }

    init(_ makeView: @escaping @autoclosure () -> Wrapped) {
        self.makeView = makeView
        self.update = { _, _ in }
    }
}

struct IdentifiableString : Identifiable {
    let string: String
    var id: String { string }
}

class ObservableStringCollection: ObservableObject {
    @Published var items = [IdentifiableString]()
}

class TestSetupUIObserverObservable : ObservableObject {
    @Published var observer : TestSetupUIObserver? = nil
}

class TestUIObserverObservable : ObservableObject {
    @Published var observer : TestUIObserver? = nil
}

class ObservableBool : ObservableObject {
    @Published var value = false
}

class ObservableString : ObservableObject {
    @Published var string = ""
}

struct SwiftSessionView : View {
    @ObservedObject var audioDevice: ObservableString
    @ObservedObject var showErrorMessage: ObservableBool
    @ObservedObject var errorMessage: ObservableString
    @ObservedObject var audioDevices: ObservableStringCollection
    
    init(ui: SwiftSessionUI) {
        audioDevices = ui.audioDevices
        audioDevice = ui.audioDevice_
        errorMessage = ui.errorMessage
        showErrorMessage = ui.showErrorMessage
    }
    
    var body: some View {
        Picker("Audio Device", selection: $audioDevice.string) {
            ForEach(audioDevices.items) {
                Text($0.string)
            }
        }
        .alert(isPresented: $showErrorMessage.value) {
            Alert(
                title: Text("Error"),
                message: Text(errorMessage.string)
            )
        }
    }
}

class SwiftSessionUI : NSObject, SessionUI {
    var showErrorMessage = ObservableBool()
    var errorMessage = ObservableString()
    var audioDevices = ObservableStringCollection()
    var audioDevice_ = ObservableString()
    
    func eventLoop() {}
    
    func showErrorMessage(_ something: String!) {
        showErrorMessage.value = true
        errorMessage.string = something
    }
    
    func audioDevice() -> String! {
        return audioDevice_.string
    }
    
    func populateAudioDeviceMenu(_ devices: [String]!) {
        for each in devices {
            audioDevices.items.append(IdentifiableString(string: each))
        }
    }
}

struct SwiftTestSetupView: View {
    @ObservedObject var testerId: ObservableString
    @ObservedObject var subjectId: ObservableString
    @ObservedObject var session: ObservableString
    @ObservedObject var startingSnr: ObservableString
    @ObservedObject var transducer: ObservableString
    @ObservedObject var rmeSetting: ObservableString
    @ObservedObject var transducers: ObservableStringCollection
    @ObservedObject var observableObserver: TestSetupUIObserverObservable
    let testSettingsPathControl: NSPathControl
    
    init(ui: SwiftTestSetupUI, testSettingsPathControl: NSPathControl){
        transducers = ui.transducers
        rmeSetting = ui.rmeSetting_
        testerId = ui.testerId_
        subjectId = ui.subjectId_
        session = ui.session_
        startingSnr = ui.startingSnr_
        transducer = ui.transducer_
        observableObserver = ui.observableObserver
        self.testSettingsPathControl = testSettingsPathControl
        self.testSettingsPathControl.pathStyle = NSPathControl.Style.popUp
        self.testSettingsPathControl.allowedTypes = ["txt"]
    }
    
    var body: some View {
        Form() {
            TextField(
                "subject ID",
                text: $subjectId.string)
                .disableAutocorrection(true)
            TextField(
                "tester ID",
                text: $testerId.string)
                .disableAutocorrection(true)
            TextField(
                "session",
                text: $session.string)
                .disableAutocorrection(true)
            TextField(
                "RME setting",
                text: $rmeSetting.string)
                .disableAutocorrection(true)
            Picker("Transducer", selection: $transducer.string) {
                ForEach(transducers.items) {
                    Text($0.string)
                }
            }
            HStack() {
                Wrap(testSettingsPathControl)
                Button(action: {
                    observableObserver.observer?.notifyThatPlayCalibrationButtonHasBeenClicked()
                }) {
                    Text("Play Calibration")
                }
            }
            TextField(
                "starting SNR (dB)",
                text: $startingSnr.string)
                .disableAutocorrection(true)
        }.padding()
        Button(action: {
            observableObserver.observer?.notifyThatConfirmButtonHasBeenClicked()
        }) {
            Text("Confirm")
        }.keyboardShortcut(.defaultAction)
    }
}

class SwiftTestSetupUI : NSObject, TestSetupUI {
    let transducers = ObservableStringCollection()
    let observableObserver = TestSetupUIObserverObservable()
    let showing = ObservableBool()
    let rmeSetting_ = ObservableString()
    let testerId_ = ObservableString()
    let subjectId_ = ObservableString()
    let session_ = ObservableString()
    let startingSnr_ = ObservableString()
    let transducer_ = ObservableString()
    let testSettingsPathControl: NSPathControl
    
    init(testSettingsPathControl: NSPathControl) {
        self.testSettingsPathControl = testSettingsPathControl
        showing.value = true
    }
    
    func show() {
        showing.value = true
    }
    
    func hide() {
        showing.value = false
    }
    
    func testerId() -> String { return testerId_.string }
    
    func subjectId() -> String { return subjectId_.string }
    
    func session() -> String { return session_.string }
    
    func testSettingsFile() -> String {
        return testSettingsPathControl.url?.path ?? ""
    }
    
    func startingSnr() -> String {
        return startingSnr_.string
    }
    
    func transducer() -> String { return transducer_.string }
    
    func rmeSetting() -> String { return rmeSetting_.string }
    
    func populateTransducerMenu(_ transducers: Array<String>) {
        for transducer in transducers {
            self.transducers.items.append(IdentifiableString(string: transducer))
        }
    }
    
    func attach(_ observer: TestSetupUIObserver!) {
        observableObserver.observer = observer
    }
}

struct SwiftTestView : View {
    @ObservedObject var observableObserver: TestUIObserverObservable
    @ObservedObject var exitTestButtonDisabled: ObservableBool
    @ObservedObject var nextTrialButtonDisabled: ObservableBool
    
    init(ui: SwiftTestUI) {
        observableObserver = ui.observableObserver
        exitTestButtonDisabled = ui.exitTestButtonDisabled
        nextTrialButtonDisabled = ui.nextTrialButtonDisabled
    }
    
    var body: some View {
        Button(action: {
            observableObserver.observer?.exitTest()
        }) {
            Text("Exit Test")
        }.disabled(exitTestButtonDisabled.value)
        Button(action: {
            observableObserver.observer?.playTrial()
        }) {
            Text("Play Trial")
        }.disabled(nextTrialButtonDisabled.value)
    }
}

class SwiftTestUI : NSObject, TestUI {
    let observableObserver = TestUIObserverObservable()
    let exitTestButtonDisabled = ObservableBool()
    let nextTrialButtonDisabled = ObservableBool()
    
    func attach(_ observer: TestUIObserver!) {
        observableObserver.observer = observer
    }
    
    func showExitTestButton() {
        exitTestButtonDisabled.value = false
    }
    
    func hideExitTestButton() {
        exitTestButtonDisabled.value = true
    }
    
    func show() {
    }
    
    func hide() {
    }
    
    func display(_ something: String!) {
        
    }
    
    func secondaryDisplay(_ something: String!) {
        
    }
    
    func showNextTrialButton() {
        nextTrialButtonDisabled.value = false
    }
    
    func hideNextTrialButton() {
        nextTrialButtonDisabled.value = true
    }
    
    func showContinueTestingDialog() {
        
    }
    
    func hideContinueTestingDialog() {
        
    }
    
    func setContinueTestingDialogMessage(_ something: String!) {
        
    }
    
    func showSheet(_ something: String!) {
        
    }
}

class SwiftTestSetupUIFactory : NSObject, TestSetupUIFactory {
    let testSetupUI: TestSetupUI
    
    init(testSetupUI: TestSetupUI){
        self.testSetupUI = testSetupUI
        super.init()
    }
    
    func make(_ viewController: NSViewController!) -> TestSetupUI! {
        return testSetupUI;
    }
}

@main
struct SwiftCPPApp: App {
    let testSettingsPathControl = NSPathControl()
    let sessionUI = SwiftSessionUI()
    let sessionView: SwiftSessionView
    let testSetupUI: SwiftTestSetupUI
    let testSetupView: SwiftTestSetupView
    let testUI = SwiftTestUI()
    let testView: SwiftTestView
    @ObservedObject var showingTestSetup: ObservableBool
    
    init() {
        testSetupUI = SwiftTestSetupUI(testSettingsPathControl: testSettingsPathControl)
        sessionView = SwiftSessionView(ui: sessionUI)
        testSetupView = SwiftTestSetupView(ui: testSetupUI, testSettingsPathControl:testSettingsPathControl)
        testView = SwiftTestView(ui: testUI)
        showingTestSetup = testSetupUI.showing
        HelloWorldObjc.doEverything(SwiftTestSetupUIFactory(testSetupUI: testSetupUI), with: sessionUI)
    }
    
    var body: some Scene {
        WindowGroup {
            sessionView
            if showingTestSetup.value {
                testSetupView
            }
            else {
                testView
            }
        }
    }
}
