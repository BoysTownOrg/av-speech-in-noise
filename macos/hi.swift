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
    @Published var observer : TestSetupUIObserver! = nil
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
    var ui: SwiftTestSetupUI
    
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
                    observableObserver.observer.notifyThatPlayCalibrationButtonHasBeenClicked()
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
                observableObserver.observer.notifyThatConfirmButtonHasBeenClicked()
            }) {
                Text("Confirm")
            }
        }
    }
}

class SwiftTestSetupUI : NSObject, TestSetupUI {
    var view: SwiftTestSetupView! = nil
    var observer: TestSetupUIObserver! = nil
    var transducers = Transducers()
    var observableObserver = TestSetupUIObserverObservable()
    
    func show() {}
    
    func hide() {}
    
    func testerId() -> String { return view.testerId }
    
    func subjectId() -> String { return view.subjectId }
    
    func session() -> String { return view.session }
    
    func testSettingsFile() -> String {
        return view.testSettingsPathControl.url?.path ?? ""
    }
    
    func startingSnr() -> String {
        return view.startingSnr
    }
    
    func transducer() -> String {return view.transducer}
    
    func rmeSetting() -> String {return view.rmeSetting}
    
    func populateTransducerMenu(_ transducers: Array<String>) {
        for transducer in transducers {
            self.transducers.items.append(Transducer(name: transducer))
        }
    }
    
    func attach(_ observer: TestSetupUIObserver!) {
        observableObserver.observer = observer
    }
}

class SwiftTestSetupUIFactory : NSObject, TestSetupUIFactory {
    let testSetupUI: TestSetupUI!
    init(testSetupUI: TestSetupUI!){
        self.testSetupUI = testSetupUI
    }
    
    func make(_ viewController: NSViewController!) -> TestSetupUI! {
        return testSetupUI;
    }
}

@main
struct SwiftCPPApp: App {
    let testSetupUI = SwiftTestSetupUI()
    let testSetupView: SwiftTestSetupView
    init() {
        testSetupView = SwiftTestSetupView(ui: testSetupUI)
        HelloWorldObjc.doEverything(SwiftTestSetupUIFactory(testSetupUI: testSetupUI))
    }
    var body: some Scene {
        WindowGroup {
            testSetupView
        }
    }
}
