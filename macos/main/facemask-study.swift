import SwiftUI

@main
struct SwiftCPPApp: App {
    let sessionUI: SwiftSessionUI
    let testSetupUI = SwiftFacemaskStudyTestSetupUI()
    
    init() {
        sessionUI = SwiftSessionUI()
        AvSpeechInNoiseMain.facemaskStudy(SwiftTestSetupUIFactory(testSetupUI: testSetupUI), with: sessionUI, with: sessionUI.testUI, with: sessionUI.freeResponseUI, with: sessionUI.syllablesUI, with: sessionUI.chooseKeywordsUI, with: sessionUI.correctKeywordsUI, with: sessionUI.passFailUI)
    }
    
    var body: some Scene {
        WindowGroup {
            SwiftSessionView(ui: sessionUI, showingTestSetup: testSetupUI.showing) {
                SwiftFacemaskStudyTestSetupView(ui: testSetupUI)
            }
        }
    }
}
