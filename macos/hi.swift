import SwiftUI
import Foundation
import AppKit

struct SwiftTestSetupView: View {
    @State var testerId: String = ""
    var ui: SwiftTestSetupUI
    init(ui: SwiftTestSetupUI){
        self.ui = ui
        ui.view = self;
    }
    var body: some View {
        TextField(
            "tester ID",
             text: $testerId)
            .disableAutocorrection(true)
        Button(action: {
            ui.notifyThatConfirmButtonHasBeenClicked()
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
    func subjectId() -> String {return "a"}
    func session() -> String {return ""}
    func testSettingsFile() -> String {return "/Users/basset/Documents/FaceMaskStimuli/ClearMask_AV_corrected.txt"}
    func startingSnr() -> String {return ""}
    func transducer() -> String {return ""}
    func rmeSetting() -> String {return ""}
    func populateTransducerMenu() {}
    func notifyThatConfirmButtonHasBeenClicked() {
        observer.notifyThatConfirmButtonHasBeenClicked();
    }
    func attach(_ observer: TestSetupUIObserver!) {
        if (observer != nil){
            print("we are not nil")
        }
        else {
            print("WE ARE NIL")
        }
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
