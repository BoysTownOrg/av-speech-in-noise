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

struct IdentifiableStringCollection : Identifiable {
    var id: String
    let items: [IdentifiableString]
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

class FreeResponseUIObserverObservable : ObservableObject {
    @Published var observer : FreeResponseUIObserver? = nil
}

class SyllablesUIObserverObservable : ObservableObject {
    @Published var observer : SyllablesUIObserver? = nil
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
    @ObservedObject var exitTestButtonEnabled: ObservableBool
    @ObservedObject var nextTrialButtonEnabled: ObservableBool
    
    init(ui: SwiftTestUI) {
        observableObserver = ui.observableObserver
        exitTestButtonEnabled = ui.exitTestButtonEnabled
        nextTrialButtonEnabled = ui.nextTrialButtonEnabled
    }
    
    var body: some View {
        Button(action: {
            observableObserver.observer?.exitTest()
        }) {
            Text("Exit Test")
        }.disabled(!exitTestButtonEnabled.value)
        Button(action: {
            observableObserver.observer?.playTrial()
        }) {
            Text("Play Trial")
        }.keyboardShortcut(.defaultAction)
        .disabled(!nextTrialButtonEnabled.value)
    }
}

class SwiftTestUI : NSObject, TestUI {
    let observableObserver = TestUIObserverObservable()
    let exitTestButtonEnabled = ObservableBool()
    let nextTrialButtonEnabled = ObservableBool()
    
    func attach(_ observer: TestUIObserver!) {
        observableObserver.observer = observer
    }
    
    func showExitTestButton() {
        exitTestButtonEnabled.value = true
    }
    
    func hideExitTestButton() {
        exitTestButtonEnabled.value = false
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
        nextTrialButtonEnabled.value = true
    }
    
    func hideNextTrialButton() {
        nextTrialButtonEnabled.value = false
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

struct SwiftSyllablesView : View {
    @ObservedObject var syllable: ObservableString
    @ObservedObject var flagged: ObservableBool
    @ObservedObject var observableObserver: SyllablesUIObserverObservable
    @ObservedObject var showing: ObservableBool
    
    init(ui: SwiftSyllablesUI) {
        syllable = ui.syllable_
        flagged = ui.flagged_
        showing = ui.showing
        observableObserver = ui.observableObserver
    }
    
    private let syllables: [IdentifiableStringCollection] = [
        IdentifiableStringCollection(id: "a", items:[
            IdentifiableString(string: "B"),
            IdentifiableString(string: "D"),
            IdentifiableString(string: "G"),
            IdentifiableString(string: "F"),
            IdentifiableString(string: "Ghee"),
            IdentifiableString(string: "H"),
            IdentifiableString(string: "Yee"),
        ]),
        IdentifiableStringCollection(id: "b", items:[
            IdentifiableString(string: "K"),
            IdentifiableString(string: "L"),
            IdentifiableString(string: "M"),
            IdentifiableString(string: "N"),
            IdentifiableString(string: "P"),
            IdentifiableString(string: "R"),
            IdentifiableString(string: "Sh"),
        ]),
        IdentifiableStringCollection(id: "c", items:[
            IdentifiableString(string: "S"),
            IdentifiableString(string: "Th"),
            IdentifiableString(string: "T"),
            IdentifiableString(string: "Ch"),
            IdentifiableString(string: "V"),
            IdentifiableString(string: "W"),
            IdentifiableString(string: "Z")
        ])
    ]
    
    var body: some View {
        if showing.value {
            Toggle("flagged", isOn: $flagged.value)
            ForEach(syllables, content: { row in
                HStack() {
                    ForEach(row.items, content: { column in
                        Button(column.string, action: {
                            syllable.string = column.string
                            observableObserver.observer?.notifyThatResponseButtonHasBeenClicked()
                        })
                    })
                }
            })
        }
    }
}

class SwiftSyllablesUI : NSObject, SyllablesUI {
    let syllable_ = ObservableString()
    let flagged_ = ObservableBool()
    let observableObserver = SyllablesUIObserverObservable()
    let showing = ObservableBool()
    
    func hide() {
        showing.value = false
    }
    
    func show() {
        showing.value = true
    }
    
    func syllable() -> String! {
        return syllable_.string;
    }
    
    func flagged() -> Bool {
        return flagged_.value;
    }
    
    func clearFlag() {
        flagged_.value = false
    }
    
    func attach(_ observer: SyllablesUIObserver!) {
        observableObserver.observer = observer
    }
}

struct SwiftFreeResponseView : View {
    @ObservedObject var freeResponse: ObservableString
    @ObservedObject var flagged: ObservableBool
    @ObservedObject var observableObserver: FreeResponseUIObserverObservable
    @ObservedObject var showing: ObservableBool
    
    init(ui: SwiftFreeResponseUI) {
        freeResponse = ui.freeResponse_
        flagged = ui.flagged_
        showing = ui.showing
        observableObserver = ui.observableObserver
    }
    
    var body: some View {
        if showing.value {
            Form {
                Toggle("flagged", isOn: $flagged.value)
                TextField("response", text: $freeResponse.string)
            }.padding()
            Button(action: {
                observableObserver.observer?.notifyThatSubmitButtonHasBeenClicked()
            }) {
                Text("Submit")
            }.keyboardShortcut(.defaultAction)
        }
    }
}

class SwiftFreeResponseUI : NSObject, FreeResponseUI {
    let freeResponse_ = ObservableString()
    let flagged_ = ObservableBool()
    let observableObserver = FreeResponseUIObserverObservable()
    let showing = ObservableBool()
    
    func attach(_ observer: FreeResponseUIObserver!) {
        observableObserver.observer = observer
    }
    
    func showFreeResponseSubmission() {
        showing.value = true
    }
    
    func hideFreeResponseSubmission() {
        showing.value = false
    }
    
    func freeResponse() -> String! {
        return freeResponse_.string
    }
    
    func flagged() -> Bool {
        return flagged_.value
    }
    
    func clearFreeResponse() {
        freeResponse_.string = ""
    }
    
    func clearFlag() {
        flagged_.value = false
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
    let freeResponseUI = SwiftFreeResponseUI()
    let freeResponseView: SwiftFreeResponseView
    let syllablesUI = SwiftSyllablesUI()
    let syllablesView: SwiftSyllablesView
    @ObservedObject var showingTestSetup: ObservableBool
    
    init() {
        testSetupUI = SwiftTestSetupUI(testSettingsPathControl: testSettingsPathControl)
        sessionView = SwiftSessionView(ui: sessionUI)
        testSetupView = SwiftTestSetupView(ui: testSetupUI, testSettingsPathControl:testSettingsPathControl)
        testView = SwiftTestView(ui: testUI)
        freeResponseView = SwiftFreeResponseView(ui: freeResponseUI)
        syllablesView = SwiftSyllablesView(ui: syllablesUI)
        showingTestSetup = testSetupUI.showing
        HelloWorldObjc.doEverything(SwiftTestSetupUIFactory(testSetupUI: testSetupUI), with: sessionUI, with: testUI, with: freeResponseUI, with: syllablesUI)
    }
    
    var body: some Scene {
        WindowGroup {
            sessionView
            if showingTestSetup.value {
                testSetupView
            }
            else {
                testView
                freeResponseView
                syllablesView
            }
        }
    }
}
