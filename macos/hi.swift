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

struct SwiftTestSetupView: View {
    @State var testerId: String = ""
    @State var subjectId: String = ""
    @State var session: String = ""
    @State var testSettingsFile: String = ""
    @State var startingSnr: String = ""
    @State var transducer: String = ""
    @State var rmeSetting: String = ""
    @ObservedObject var transducers = Transducers()
    let testSettingsPathControl = NSPathControl()
    var ui: SwiftTestSetupUI
    
    init(ui: SwiftTestSetupUI){
        self.ui = ui
        testSettingsPathControl.pathStyle = NSPathControl.Style.popUp
        testSettingsPathControl.allowedTypes = ["txt"]
        ui.view = self
    }
    
    var body: some View {
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
        TextField(
            "transducer",
            text: $transducer)
            .disableAutocorrection(true)
        Picker("Flavor", selection: $transducer) {
            ForEach(transducers.items) {
                Text($0.name)
            }
        }
        Wrap(testSettingsPathControl)
        TextField(
            "starting SNR (dB)",
            text: $startingSnr)
            .disableAutocorrection(true)
        Button(action: {
            print(startingSnr)
            ui.notifyThatConfirmButtonHasBeenClicked(view: self)
        }) {
            Text("Confirm")
        }
    }
}

class SwiftTestSetupUI : NSObject, TestSetupUI {
    var view: SwiftTestSetupView! = nil
    var observer: TestSetupUIObserver! = nil
    
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
            view.transducers.items.append(Transducer(name: transducer))
        }
    }
    
    func notifyThatConfirmButtonHasBeenClicked(view: SwiftTestSetupView) {
        self.view = view
        observer.notifyThatConfirmButtonHasBeenClicked();
    }
    
    func attach(_ observer: TestSetupUIObserver!) {
        self.observer = observer
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

class MySwiftClass : NSObject, PrintProtocolDelegate {
    func someFunction() -> String {
        return HelloWorldObjc().sayHello(self)
    }
    
    func favoriteNumber() -> NSInteger {
        return 42;
    }
}

struct ContentView: View {
    var body: some View {
        Text(MySwiftClass().someFunction()).padding()
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
