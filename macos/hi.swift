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

class TestSetupUIObserverObservable : ObservableObject {
    @Published var observer : TestSetupUIObserver? = nil
}

class TestUIObserverObservable : ObservableObject {
    @Published var observer : TestUIObserver? = nil
}

class ObservableBool : ObservableObject {
    @Published var value = false
}

struct SwiftTestSetupView: View {
    @State var testerId: String = ""
    @State var subjectId: String = ""
    @State var session: String = ""
    @State var testSettingsFile: String = ""
    @State var startingSnr: String = ""
    @State var transducer: String = ""
    @State var rmeSetting: String = ""
    @ObservedObject var transducers: Transducers
    @ObservedObject var observableObserver: TestSetupUIObserverObservable
    let testSettingsPathControl = NSPathControl()
    let ui: SwiftTestSetupUI
    
    init(ui: SwiftTestSetupUI){
        self.ui = ui
        transducers = ui.transducers
        observableObserver = ui.observableObserver
        testSettingsPathControl.pathStyle = NSPathControl.Style.popUp
        testSettingsPathControl.allowedTypes = ["txt"]
    }
    
    var body: some View {
        Form() {
            TextField(
                "subject ID",
                text: $subjectId)
                .disableAutocorrection(true)
            TextField(
                "tester ID",
                text: $testerId)
                .disableAutocorrection(true)
            TextField(
                "session",
                text: $session)
                .disableAutocorrection(true)
            TextField(
                "RME setting",
                text: $rmeSetting)
                .disableAutocorrection(true)
            Picker("Transducer", selection: $transducer) {
                ForEach(transducers.items) {
                    Text($0.name)
                }
            }
            HStack() {
                Wrap(testSettingsPathControl)
                Button(action: {
                    self.ui.view = self
                    observableObserver.observer?.notifyThatPlayCalibrationButtonHasBeenClicked()
                }) {
                    Text("Play Calibration")
                }
            }
            TextField(
                "starting SNR (dB)",
                text: $startingSnr)
                .disableAutocorrection(true)
            Button(action: {
                self.ui.view = self
                observableObserver.observer?.notifyThatConfirmButtonHasBeenClicked()
            }) {
                Text("Confirm")
            }
        }
    }
}

class SwiftTestSetupUI : NSObject, TestSetupUI {
    var view: SwiftTestSetupView? = nil
    let transducers = Transducers()
    let observableObserver = TestSetupUIObserverObservable()
    var showing = ObservableBool()
    
    override init() {
        showing.value = true
        super.init()
    }
    
    func show() {
        showing.value = true
    }
    
    func hide() {
        showing.value = false
    }
    
    func testerId() -> String { return view?.testerId ?? "" }
    
    func subjectId() -> String { return view?.subjectId ?? "" }
    
    func session() -> String { return view?.session ?? "" }
    
    func testSettingsFile() -> String {
        return view?.testSettingsPathControl.url?.path ?? ""
    }
    
    func startingSnr() -> String {
        return view?.startingSnr ?? ""
    }
    
    func transducer() -> String {return view?.transducer ?? ""}
    
    func rmeSetting() -> String {return view?.rmeSetting ?? ""}
    
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
    let testSetupUI = SwiftTestSetupUI()
    let testSetupView: SwiftTestSetupView
    let testUI = SwiftTestUI()
    let testView: SwiftTestView
    @ObservedObject var showingTestSetup: ObservableBool
    
    init() {
        testSetupView = SwiftTestSetupView(ui: testSetupUI)
        testView = SwiftTestView(ui: testUI)
        showingTestSetup = testSetupUI.showing
        HelloWorldObjc.doEverything(SwiftTestSetupUIFactory(testSetupUI: testSetupUI))
    }
    
    var body: some Scene {
        WindowGroup {
            if showingTestSetup.value {
                testSetupView
            }
            else {
                testView
            }
        }
    }
}
