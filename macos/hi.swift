import SwiftUI
import Foundation
import AppKit

struct SwiftTestSetupView: View {
    @State var testerId: String = ""
    var body: some View {
        TextField(
            "tester ID",
             text: $testerId)
            .disableAutocorrection(true)
    }
}

class SwiftTestSetupUI : NSObject, TestSetupUI {
    let view: SwiftTestSetupView
    init(view: SwiftTestSetupView){
        self.view = view
    }
    func show() {}
    func hide() {}
    func testerId() -> String { return view.testerId }
    func subjectId() -> String {return ""}
    func session() -> String {return ""}
    func testSettingsFile() -> String {return ""}
    func startingSnr() -> String {return ""}
    func transducer() -> String {return ""}
    func rmeSetting() -> String {return ""}
    func populateTransducerMenu() {}
    func attach() {}
}

class SwiftTestSetupUIFactory : NSObject, TestSetupUIFactory {
    let view: SwiftTestSetupView
    init(view: SwiftTestSetupView){
        self.view = view
    }

    func make(_ viewController: NSViewController!) -> TestSetupUI! {
        return SwiftTestSetupUI(view: view);
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
    let testSetupView = SwiftTestSetupView()
    init() {HelloWorldObjc.doEverything(SwiftTestSetupUIFactory(view: testSetupView))}
    var body: some Scene {
        WindowGroup {
            testSetupView
        }
    }
}