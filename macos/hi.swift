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

struct Transducer : Identifiable {
    let name: String
    var id: String { name }
}

class Transducers: ObservableObject {
    @Published var items = [Transducer]()
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
    @ObservedObject var transducers: Transducers
    @ObservedObject var observableObserver: TestSetupUIObserverObservable
    let testSettingsPathControl: NSPathControl
    
    init(ui: SwiftTestSetupUI, testSettingsPathControl: NSPathControl){
        self.testSettingsPathControl = testSettingsPathControl
        transducers = ui.transducers
        rmeSetting = ui.rmeSetting_
        testerId = ui.testerId_
        subjectId = ui.subjectId_
        session = ui.session_
        startingSnr = ui.startingSnr_
        transducer = ui.transducer_
        observableObserver = ui.observableObserver
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
                    Text($0.name)
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
    let transducers = Transducers()
    let observableObserver = TestSetupUIObserverObservable()
    var showing = ObservableBool()
    let testSettingsPathControl: NSPathControl
    var rmeSetting_ = ObservableString()
    var testerId_ = ObservableString()
    var subjectId_ = ObservableString()
    var session_ = ObservableString()
    var startingSnr_ = ObservableString()
    var transducer_ = ObservableString()
    
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
    
    func transducer() -> String {return transducer_.string }
    
    func rmeSetting() -> String {return rmeSetting_.string }
    
    func populateTransducerMenu(_ transducers: Array<String>) {
        for transducer in transducers {
            self.transducers.items.append(Transducer(name: transducer))
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
    var observableObserver = TestUIObserverObservable()
    var exitTestButtonDisabled = ObservableBool()
    var nextTrialButtonDisabled = ObservableBool()
    
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
    let testSetupUI: TestSetupUI?
    
    init(testSetupUI: TestSetupUI?){
        self.testSetupUI = testSetupUI
        super.init()
    }
    
    func make(_ viewController: NSViewController!) -> TestSetupUI! {
        return testSetupUI;
    }
}

@main
struct SwiftCPPApp: App {
    let sessionUI = SwiftSessionUI()
    let sessionView: SwiftSessionView
    let testSettingsPathControl = NSPathControl()
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
        HelloWorldObjc.doEverything(SwiftTestSetupUIFactory(testSetupUI: testSetupUI))
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
