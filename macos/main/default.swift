import SwiftUI

@main
struct SwiftCPPApp: App {
    let testSettingsPathControl = NSPathControl()
    let sessionUI: SwiftSessionUI
    let testSetupUI: SwiftTestSetupUI
    
    init() {
        testSetupUI = SwiftTestSetupUI(testSettingsPathControl: testSettingsPathControl)
        sessionUI = SwiftSessionUI(testSetupUI: testSetupUI)
    }
    
    var body: some Scene {
        WindowGroup {
            SwiftSessionView(ui: sessionUI, showingTestSetup: testSetupUI.showing) {
                SwiftTestSetupView(ui: testSetupUI, testSettingsPathControl:testSettingsPathControl)
            }
        }
    }
}
