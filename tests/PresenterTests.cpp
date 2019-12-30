#include "PresenterTests.h"

namespace av_speech_in_noise::tests::presentation {
TEST_F(PresenterTests, populatesConditionMenu) {
    assertSetupViewConditionsContains(auditoryOnlyConditionName());
    assertSetupViewConditionsContains(audioVisualConditionName());
}

TEST_F(PresenterTests, populatesMethodMenu) {
    assertSetupViewMethodsContains(Method::fixedLevelOpenSet);
    assertSetupViewMethodsContains(Method::fixedLevelClosedSet);
    assertSetupViewMethodsContains(Method::adaptiveOpenSet);
    assertSetupViewMethodsContains(Method::adaptiveClosedSet);
    assertSetupViewMethodsContains(Method::adaptiveClosedSetSingleSpeaker);
    assertSetupViewMethodsContains(Method::adaptiveClosedSetDelayedMasker);
}

TEST_F(PresenterTests, callsEventLoopWhenRun) {
    presenter.run();
    assertTrue(view.eventLoopCalled());
}

TEST_F(PresenterTests, confirmAdaptiveClosedSetTestHidesTestSetupView) {
    assertHidesTestSetupView(confirmingAdaptiveClosedSetTest);
}

TEST_F(PresenterTests,
    confirmAdaptiveClosedSetSingleSpeakerTestHidesTestSetupView) {
    assertHidesTestSetupView(confirmingAdaptiveClosedSetSingleSpeakerTest);
}

TEST_F(PresenterTests,
    confirmAdaptiveClosedSetDelayedMaskerTestHidesTestSetupView) {
    assertHidesTestSetupView(confirmingAdaptiveClosedSetDelayedMaskerTest);
}

TEST_F(PresenterTests, confirmAdaptiveOpenSetTestHidesTestSetupView) {
    assertHidesTestSetupView(confirmingAdaptiveOpenSetTest);
}

TEST_F(PresenterTests, confirmFixedLevelOpenSetTestHidesTestSetupView) {
    assertHidesTestSetupView(confirmingFixedLevelOpenSetTest);
}

TEST_F(PresenterTests, confirmFixedLevelClosedSetTestHidesTestSetupView) {
    assertHidesTestSetupView(confirmingFixedLevelClosedSetTest);
}

TEST_F(PresenterTests, confirmAdaptiveOpenSetTestShowsExperimenterView) {
    assertShowsExperimenterView(confirmingAdaptiveOpenSetTest);
}

TEST_F(PresenterTests, confirmAdaptiveOpenSetTestShowsTestingView) {
    assertShowsTestingView(confirmingAdaptiveOpenSetTest);
}

TEST_F(PresenterTests, confirmFixedLevelOpenSetTestShowsTestingView) {
    assertShowsTestingView(confirmingFixedLevelOpenSetTest);
}

TEST_F(PresenterTests, confirmFixedLevelOpenSetTestShowsExperimenterView) {
    assertShowsExperimenterView(confirmingFixedLevelOpenSetTest);
}

TEST_F(PresenterTests, confirmAdaptiveClosedSetTestShowsSubjectView) {
    assertShowsSubjectView(confirmingAdaptiveClosedSetTest);
}

TEST_F(
    PresenterTests, confirmAdaptiveClosedSetSingleSpeakerTestShowsSubjectView) {
    assertShowsSubjectView(confirmingAdaptiveClosedSetSingleSpeakerTest);
}

TEST_F(
    PresenterTests, confirmAdaptiveClosedSetDelayedMaskerTestShowsSubjectView) {
    assertShowsSubjectView(confirmingAdaptiveClosedSetDelayedMaskerTest);
}

TEST_F(PresenterTests,
    confirmAdaptiveClosedSetTestDoesNotShowSubjectViewWhenTestComplete) {
    setTestComplete();
    assertDoesNotShowSubjectView(confirmingAdaptiveClosedSetTest);
}

TEST_F(PresenterTests,
    confirmAdaptiveClosedSetTestDoesNotHideSetupViewWhenTestComplete) {
    setTestComplete();
    assertDoesNotHideTestSetupView(confirmingAdaptiveClosedSetTest);
}

TEST_F(PresenterTests, confirmFixedLevelClosedSetTestShowsSubjectView) {
    assertShowsSubjectView(confirmingFixedLevelClosedSetTest);
}

TEST_F(PresenterTests, confirmAdaptiveOpenSetTestDoesNotShowSubjectView) {
    assertDoesNotShowSubjectView(confirmingAdaptiveOpenSetTest);
}

TEST_F(PresenterTests, confirmFixedLevelTestDoesNotShowSubjectView) {
    assertDoesNotShowSubjectView(confirmingFixedLevelOpenSetTest);
}

TEST_F(PresenterTests, confirmAdaptiveClosedSetTestShowsExperimenterView) {
    assertShowsExperimenterView(confirmingAdaptiveClosedSetTest);
}

TEST_F(PresenterTests, confirmFixedLevelClosedSetTestShowsExperimenterView) {
    assertShowsExperimenterView(confirmingFixedLevelClosedSetTest);
}

TEST_F(PresenterTests,
    confirmAdaptiveClosedSetTestWithSingleSpeakerInitializesModel) {
    run(confirmingAdaptiveClosedSetSingleSpeakerTest);
    assertTrue(model.initializedWithSingleSpeaker());
}

TEST_F(PresenterTests,
    confirmAdaptiveClosedSetTestWithDelayedMaskerInitializesModel) {
    run(confirmingAdaptiveClosedSetDelayedMaskerTest);
    assertTrue(model.initializedWithDelayedMasker());
}

TEST_F(PresenterTests,
    confirmFixedLevelClosedSetTestWithFiniteTargetsInitializesModel) {
    setupView.useFiniteTargets();
    run(confirmingFixedLevelClosedSetTest);
    assertTrue(model.initializedWithFiniteTargets());
}

TEST_F(PresenterTests,
    confirmAdaptiveClosedSetTestDoesNotInitializeFixedLevelTest) {
    assertDoesNotInitializeFixedLevelTest(confirmingAdaptiveClosedSetTest);
}

TEST_F(
    PresenterTests, confirmAdaptiveOpenSetTestDoesNotInitializeFixedLevelTest) {
    assertDoesNotInitializeFixedLevelTest(confirmingAdaptiveOpenSetTest);
}

TEST_F(
    PresenterTests, confirmFixedLevelOpenSetTestDoesNotInitializeAdaptiveTest) {
    assertDoesNotInitializeAdaptiveTest(confirmingFixedLevelOpenSetTest);
}

TEST_F(PresenterTests,
    confirmFixedLevelClosedSetTestDoesNotInitializeAdaptiveTest) {
    assertDoesNotInitializeAdaptiveTest(confirmingFixedLevelClosedSetTest);
}

TEST_F(PresenterTests, confirmingAdaptiveClosedSetTestPassesStartingSnr) {
    assertStartingSnrPassedToModel(confirmingAdaptiveClosedSetTest);
}

TEST_F(PresenterTests, confirmingAdaptiveOpenSetTestPassesStartingSnr) {
    assertStartingSnrPassedToModel(confirmingAdaptiveOpenSetTest);
}

TEST_F(PresenterTests, confirmFixedLevelOpenSetTestPassesStartingSnr) {
    assertStartingSnrPassedToModel(confirmingFixedLevelOpenSetTest);
}

TEST_F(PresenterTests, confirmFixedLevelClosedSetTestPassesStartingSnr) {
    assertStartingSnrPassedToModel(confirmingFixedLevelClosedSetTest);
}

TEST_F(PresenterTests,
    confirmFixedLevelClosedSetTestWithFiniteTargetsPassesStartingSnr) {
    setStartingSnr("1");
    setMethod(&setupView, Method::fixedLevelClosedSet);
    setupView.useFiniteTargets();
    setupView.confirmTestSetup();
    assertEqual(1, model.fixedLevelTest().snr_dB);
}

TEST_F(PresenterTests, confirmingAdaptiveClosedSetTestPassesMaskerLevel) {
    assertMaskerLevelPassedToModel(confirmingAdaptiveClosedSetTest);
}

TEST_F(PresenterTests, confirmingAdaptiveOpenSetTestPassesMaskerLevel) {
    assertMaskerLevelPassedToModel(confirmingAdaptiveOpenSetTest);
}

TEST_F(PresenterTests, confirmFixedLevelOpenSetTestPassesMaskerLevel) {
    assertMaskerLevelPassedToModel(confirmingFixedLevelOpenSetTest);
}

TEST_F(PresenterTests, confirmFixedLevelClosedSetTestPassesMaskerLevel) {
    assertMaskerLevelPassedToModel(confirmingFixedLevelClosedSetTest);
}

TEST_F(PresenterTests, playCalibrationPassesLevel) {
    setCalibrationLevel("1");
    playCalibration();
    assertEqual(1, calibration().level_dB_SPL);
}

TEST_F(PresenterTests, confirmingAdaptiveClosedSetTestPassesTargetList) {
    assertPassesTargetListDirectory(confirmingAdaptiveClosedSetTest);
}

TEST_F(PresenterTests, confirmingAdaptiveOpenSetTestPassesTargetList) {
    assertPassesTargetListDirectory(confirmingAdaptiveOpenSetTest);
}

TEST_F(PresenterTests, confirmingFixedLevelOpenSetTestPassesTargetList) {
    assertPassesTargetListDirectory(confirmingFixedLevelOpenSetTest);
}

TEST_F(PresenterTests, confirmingFixedLevelClosedSetTestPassesTargetList) {
    assertPassesTargetListDirectory(confirmingFixedLevelClosedSetTest);
}

TEST_F(PresenterTests, confirmingAdaptiveClosedSetTestPassesSubjectId) {
    assertPassesSubjectId(confirmingAdaptiveClosedSetTest);
}

TEST_F(PresenterTests, confirmingAdaptiveOpenSetTestPassesSubjectId) {
    assertPassesSubjectId(confirmingAdaptiveOpenSetTest);
}

TEST_F(PresenterTests, confirmingFixedLevelOpenSetTestPassesSubjectId) {
    assertPassesSubjectId(confirmingFixedLevelOpenSetTest);
}

TEST_F(PresenterTests, confirmingFixedLevelClosedSetTestPassesSubjectId) {
    assertPassesSubjectId(confirmingFixedLevelClosedSetTest);
}

TEST_F(PresenterTests, confirmingAdaptiveClosedSetTestPassesTesterId) {
    assertPassesTesterId(confirmingAdaptiveClosedSetTest);
}

TEST_F(PresenterTests, confirmingAdaptiveOpenSetTestPassesTesterId) {
    assertPassesTesterId(confirmingAdaptiveOpenSetTest);
}

TEST_F(PresenterTests, confirmingFixedLevelOpenSetTestPassesTesterId) {
    assertPassesTesterId(confirmingFixedLevelOpenSetTest);
}

TEST_F(PresenterTests, confirmingFixedLevelClosedSetTestPassesTesterId) {
    assertPassesTesterId(confirmingFixedLevelClosedSetTest);
}

TEST_F(PresenterTests, confirmingAdaptiveClosedSetTestPassesMasker) {
    assertPassesMasker(confirmingAdaptiveClosedSetTest);
}

TEST_F(PresenterTests, confirmingAdaptiveOpenSetTestPassesMasker) {
    assertPassesMasker(confirmingAdaptiveOpenSetTest);
}

TEST_F(PresenterTests, confirmingFixedLevelOpenSetTestPassesMasker) {
    assertPassesMasker(confirmingFixedLevelOpenSetTest);
}

TEST_F(PresenterTests, confirmingFixedLevelClosedSetTestPassesMasker) {
    assertPassesMasker(confirmingFixedLevelClosedSetTest);
}

TEST_F(PresenterTests, playCalibrationPassesFilePath) {
    setupView.setCalibrationFilePath("a");
    playCalibration();
    assertEqual("a", calibration().filePath);
}

TEST_F(PresenterTests, confirmingAdaptiveClosedSetTestPassesSession) {
    assertPassesSession(confirmingAdaptiveClosedSetTest);
}

TEST_F(PresenterTests, confirmingAdaptiveOpenSetTestPassesSession) {
    assertPassesSession(confirmingAdaptiveOpenSetTest);
}

TEST_F(PresenterTests, confirmingFixedLevelOpenSetTestPassesSession) {
    assertPassesSession(confirmingFixedLevelOpenSetTest);
}

TEST_F(PresenterTests, confirmingFixedLevelClosedSetTestPassesSession) {
    assertPassesSession(confirmingFixedLevelClosedSetTest);
}

TEST_F(PresenterTests, confirmingAdaptiveClosedSetTestPassesMethod) {
    assertPassesMethod(confirmingAdaptiveClosedSetTest);
}

TEST_F(PresenterTests, confirmingAdaptiveOpenSetTestPassesMethod) {
    assertPassesMethod(confirmingAdaptiveOpenSetTest);
}

TEST_F(PresenterTests, confirmingFixedLevelOpenSetTestPassesMethod) {
    assertPassesMethod(confirmingFixedLevelOpenSetTest);
}

TEST_F(PresenterTests, confirmingFixedLevelClosedSetTestPassesMethod) {
    assertPassesMethod(confirmingFixedLevelClosedSetTest);
}

TEST_F(PresenterTests, confirmingAdaptiveClosedSetDelayedMaskerTestPassesMethod) {
    assertPassesMethod(confirmingAdaptiveClosedSetDelayedMaskerTest);
}

TEST_F(PresenterTests, confirmingAdaptiveClosedSetSingleSpeakerTestPassesMethod) {
    assertPassesMethod(confirmingAdaptiveClosedSetSingleSpeakerTest);
}

TEST_F(PresenterTests, confirmingAdaptiveClosedSetTestPassesCeilingSNR) {
    assertPassesCeilingSNR(confirmingAdaptiveClosedSetTest);
}

TEST_F(PresenterTests, confirmingAdaptiveOpenSetTestPassesCeilingSNR) {
    assertPassesCeilingSNR(confirmingAdaptiveOpenSetTest);
}

TEST_F(PresenterTests, confirmingAdaptiveClosedSetTestPassesFloorSNR) {
    assertPassesFloorSNR(confirmingAdaptiveClosedSetTest);
}

TEST_F(PresenterTests, confirmingAdaptiveOpenSetTestPassesFloorSNR) {
    assertPassesFloorSNR(confirmingAdaptiveOpenSetTest);
}

TEST_F(PresenterTests, confirmingAdaptiveClosedSetTestPassesTrackBumpLimit) {
    assertPassesTrackBumpLimit(confirmingAdaptiveClosedSetTest);
}

TEST_F(PresenterTests, confirmingAdaptiveOpenSetTestPassesTrackBumpLimit) {
    assertPassesTrackBumpLimit(confirmingAdaptiveOpenSetTest);
}

TEST_F(PresenterTests, confirmingAdaptiveClosedSetTestPassesFullScaleLevel) {
    assertPassesFullScaleLevel(confirmingAdaptiveClosedSetTest);
}

TEST_F(PresenterTests, confirmingAdaptiveOpenSetTestPassesFullScaleLevel) {
    assertPassesFullScaleLevel(confirmingAdaptiveOpenSetTest);
}

TEST_F(PresenterTests, confirmingFixedLevelOpenSetTestPassesFullScaleLevel) {
    assertPassesFullScaleLevel(confirmingFixedLevelOpenSetTest);
}

TEST_F(PresenterTests, confirmingFixedLevelClosedSetTestPassesFullScaleLevel) {
    assertPassesFullScaleLevel(confirmingFixedLevelClosedSetTest);
}

TEST_F(PresenterTests, playCalibrationPassesFullScaleLevel) {
    assertPassesFullScaleLevel(playingCalibration);
}

TEST_F(PresenterTests, confirmingAdaptiveClosedSetTestPassesTrackSettingsFile) {
    assertPassesTrackSettingsFile(confirmingAdaptiveClosedSetTest);
}

TEST_F(PresenterTests, confirmingAdaptiveOpenSetTestPassesTrackSettingsFile) {
    assertPassesTrackSettingsFile(confirmingAdaptiveOpenSetTest);
}

TEST_F(
    PresenterTests, confirmingAdaptiveClosedSetTestPassesAudioVisualCondition) {
    assertAudioVisualConditionPassedToModel(confirmingAdaptiveClosedSetTest);
}

TEST_F(PresenterTests,
    confirmingAdaptiveClosedSetTestPassesAuditoryOnlyCondition) {
    assertAuditoryOnlyConditionPassedToModel(confirmingAdaptiveClosedSetTest);
}

TEST_F(
    PresenterTests, confirmingAdaptiveOpenSetTestPassesAudioVisualCondition) {
    assertAudioVisualConditionPassedToModel(confirmingAdaptiveOpenSetTest);
}

TEST_F(
    PresenterTests, confirmingAdaptiveOpenSetTestPassesAuditoryOnlyCondition) {
    assertAuditoryOnlyConditionPassedToModel(confirmingAdaptiveOpenSetTest);
}

TEST_F(
    PresenterTests, confirmingFixedLevelOpenSetTestPassesAudioVisualCondition) {
    assertAudioVisualConditionPassedToModel(confirmingFixedLevelOpenSetTest);
}

TEST_F(PresenterTests,
    confirmingFixedLevelOpenSetTestPassesAuditoryOnlyCondition) {
    assertAuditoryOnlyConditionPassedToModel(confirmingFixedLevelOpenSetTest);
}

TEST_F(PresenterTests,
    confirmingFixedLevelClosedSetTestPassesAudioVisualCondition) {
    assertAudioVisualConditionPassedToModel(confirmingFixedLevelClosedSetTest);
}

TEST_F(PresenterTests,
    confirmingFixedLevelClosedetTestPassesAuditoryOnlyCondition) {
    assertAuditoryOnlyConditionPassedToModel(confirmingFixedLevelClosedSetTest);
}

TEST_F(PresenterTests, playCalibrationPassesAudioVisualCondition) {
    assertAudioVisualConditionPassedToModel(playingCalibration);
}

TEST_F(PresenterTests, playCalibrationPassesAuditoryOnlyCondition) {
    assertAuditoryOnlyConditionPassedToModel(playingCalibration);
}

TEST_F(PresenterTests,
    confirmAdaptiveClosedSetTestShowsNextTrialButtonForSubject) {
    assertConfirmTestSetupShowsNextTrialButton(
        confirmingAdaptiveClosedSetTest, playingTrialFromSubject);
}

TEST_F(PresenterTests,
    confirmFixedLevelClosedSetTestShowsNextTrialButtonForSubject) {
    assertConfirmTestSetupShowsNextTrialButton(
        confirmingFixedLevelClosedSetTest, playingTrialFromSubject);
}

TEST_F(PresenterTests,
    confirmAdaptiveOpenSetTestShowsNextTrialButtonForExperimenter) {
    assertConfirmTestSetupShowsNextTrialButton(
        confirmingAdaptiveOpenSetTest, playingTrialFromExperimenter);
}

TEST_F(PresenterTests,
    confirmFixedLevelOpenSetTestShowsNextTrialButtonForExperimenter) {
    assertConfirmTestSetupShowsNextTrialButton(
        confirmingFixedLevelOpenSetTest, playingTrialFromExperimenter);
}

TEST_F(PresenterTests,
    confirmingAdaptiveClosedSetTestWithInvalidMaskerLevelShowsErrorMessage) {
    assertInvalidMaskerLevelShowsErrorMessage(confirmingAdaptiveClosedSetTest);
}

TEST_F(PresenterTests,
    confirmingAdaptiveOpenSetTestWithInvalidMaskerLevelShowsErrorMessage) {
    assertInvalidMaskerLevelShowsErrorMessage(confirmingAdaptiveOpenSetTest);
}

TEST_F(PresenterTests,
    confirmingFixedLevelOpenSetTestWithInvalidMaskerLevelShowsErrorMessage) {
    assertInvalidMaskerLevelShowsErrorMessage(confirmingFixedLevelOpenSetTest);
}

TEST_F(PresenterTests,
    confirmingFixedLevelClosedSetTestWithInvalidMaskerLevelShowsErrorMessage) {
    assertInvalidMaskerLevelShowsErrorMessage(
        confirmingFixedLevelClosedSetTest);
}

TEST_F(PresenterTests, playCalibrationWithInvalidLevelShowsErrorMessage) {
    assertInvalidCalibrationLevelShowsErrorMessage(playingCalibration);
}

TEST_F(PresenterTests, respondingFromSubjectPlaysTrial) {
    assertPlaysTrial(respondingFromSubject);
}

TEST_F(PresenterTests, playingTrialFromSubjectPlaysTrial) {
    assertPlaysTrial(playingTrialFromSubject);
}

TEST_F(PresenterTests, playingTrialFromExperimenterPlaysTrial) {
    assertPlaysTrial(playingTrialFromExperimenter);
}

TEST_F(PresenterTests, playingTrialHidesNextTrialButton) {
    assertHidesPlayTrialButton(playingTrialFromSubject);
}

TEST_F(PresenterTests, playingTrialHidesNextTrialButtonForExperimenter) {
    assertHidesPlayTrialButton(playingTrialFromExperimenter);
}

TEST_F(PresenterTests, playingTrialFromSubjectHidesExitTestButton) {
    assertHidesExitTestButton(playingTrialFromSubject);
}

TEST_F(PresenterTests, playingTrialFromSubjectPassesAudioDevice) {
    assertAudioDevicePassedToTrial(playingTrialFromSubject);
}

TEST_F(PresenterTests, playingTrialFromExperimenterPassesAudioDevice) {
    assertAudioDevicePassedToTrial(playingTrialFromExperimenter);
}

TEST_F(PresenterTests, playCalibrationPassesAudioDevice) {
    setAudioDevice("b");
    playCalibration();
    assertEqual("b", calibration().audioSettings.audioDevice);
}

TEST_F(PresenterTests, subjectResponsePassesNumberResponse) {
    subjectView.setNumberResponse("1");
    respondFromSubject();
    assertEqual(1, model.responseParameters().number);
}

TEST_F(PresenterTests, subjectResponsePassesGreenColor) {
    subjectView.setGreenResponse();
    respondFromSubject();
    assertModelPassedCondition(coordinate_response_measure::Color::green);
}

TEST_F(PresenterTests, subjectResponsePassesRedColor) {
    subjectView.setRedResponse();
    respondFromSubject();
    assertModelPassedCondition(coordinate_response_measure::Color::red);
}

TEST_F(PresenterTests, subjectResponsePassesBlueColor) {
    subjectView.setBlueResponse();
    respondFromSubject();
    assertModelPassedCondition(coordinate_response_measure::Color::blue);
}

TEST_F(PresenterTests, subjectResponsePassesWhiteColor) {
    subjectView.setGrayResponse();
    respondFromSubject();
    assertModelPassedCondition(coordinate_response_measure::Color::white);
}

TEST_F(PresenterTests, experimenterResponsePassesResponse) {
    testingView.setResponse("a");
    respondFromExperimenter();
    assertEqual("a", model.freeResponse().response);
}

TEST_F(PresenterTests, experimenterResponseFlagsResponse) {
    testingView.flagResponse();
    respondFromExperimenter();
    assertTrue(model.freeResponse().flagged);
}

TEST_F(PresenterTests, passedTrialSubmitsCorrectResponse) {
    run(submittingPassedTrial);
    assertTrue(model.correctResponseSubmitted());
}

TEST_F(PresenterTests, failedTrialSubmitsIncorrectResponse) {
    run(submittingFailedTrial);
    assertTrue(model.incorrectResponseSubmitted());
}

TEST_F(PresenterTests, respondFromSubjectShowsSetupViewWhenTestComplete) {
    assertCompleteTestShowsSetupView(respondingFromSubject);
}

TEST_F(PresenterTests, respondFromExperimenterShowsSetupViewWhenTestComplete) {
    assertCompleteTestShowsSetupView(respondingFromExperimenter);
}

TEST_F(PresenterTests, submitPassedTrialShowsSetupViewWhenTestComplete) {
    assertCompleteTestShowsSetupView(submittingPassedTrial);
}

TEST_F(PresenterTests, submitFailedTrialShowsSetupViewWhenTestComplete) {
    assertCompleteTestShowsSetupView(submittingFailedTrial);
}

TEST_F(
    PresenterTests, respondFromSubjectDoesNotShowSetupViewWhenTestIncomplete) {
    assertIncompleteTestDoesNotShowSetupView(respondingFromSubject);
}

TEST_F(PresenterTests,
    respondFromExperimenterDoesNotShowSetupViewWhenTestIncomplete) {
    assertIncompleteTestDoesNotShowSetupView(respondingFromExperimenter);
}

TEST_F(
    PresenterTests, submitPassedTrialDoesNotShowSetupViewWhenTestIncomplete) {
    assertIncompleteTestDoesNotShowSetupView(submittingPassedTrial);
}

TEST_F(
    PresenterTests, submitFailedTrialDoesNotShowSetupViewWhenTestIncomplete) {
    assertIncompleteTestDoesNotShowSetupView(submittingFailedTrial);
}

TEST_F(
    PresenterTests, respondFromSubjectHidesExperimenterViewWhenTestComplete) {
    assertCompleteTestHidesExperimenterView(respondingFromSubject);
}

TEST_F(PresenterTests,
    respondFromExperimenterHidesExperimenterViewWhenTestComplete) {
    assertCompleteTestHidesExperimenterView(respondingFromExperimenter);
}

TEST_F(PresenterTests, submitPassedTrialHidesExperimenterViewWhenTestComplete) {
    assertCompleteTestHidesExperimenterView(submittingPassedTrial);
}

TEST_F(PresenterTests, submitFailedTrialHidesExperimenterViewWhenTestComplete) {
    assertCompleteTestHidesExperimenterView(submittingFailedTrial);
}

TEST_F(
    PresenterTests, respondFromExperimenterHidesTestingViewWhenTestComplete) {
    assertCompleteTestHidesTestingView(respondingFromExperimenter);
}

TEST_F(PresenterTests, submitPassedTrialHidesTestingViewWhenTestComplete) {
    assertCompleteTestHidesTestingView(submittingPassedTrial);
}

TEST_F(PresenterTests, submitFailedTrialHidesTestingViewWhenTestComplete) {
    assertCompleteTestHidesTestingView(submittingFailedTrial);
}

TEST_F(
    PresenterTests, submitCoordinateResponseDoesNotPlayTrialWhenTestComplete) {
    assertCompleteTestDoesNotPlayTrial(respondingFromSubject);
}

TEST_F(PresenterTests,
    respondFromSubjectDoesNotHideExperimenterViewWhenTestIncomplete) {
    assertDoesNotHideExperimenterView(respondingFromSubject);
}

TEST_F(PresenterTests,
    respondFromExperimenterDoesNotHideExperimenterViewWhenTestIncomplete) {
    assertDoesNotHideExperimenterView(respondingFromExperimenter);
}

TEST_F(PresenterTests,
    submitPassedTrialDoesNotHideExperimenterViewWhenTestIncomplete) {
    assertDoesNotHideExperimenterView(submittingPassedTrial);
}

TEST_F(PresenterTests,
    submitFailedTrialDoesNotHideExperimenterViewWhenTestIncomplete) {
    assertDoesNotHideExperimenterView(submittingFailedTrial);
}

TEST_F(PresenterTests,
    respondFromExperimenterDoesNotHideTestingViewWhenTestIncomplete) {
    assertDoesNotHideTestingView(respondingFromExperimenter);
}

TEST_F(
    PresenterTests, submitPassedTrialDoesNotHideTestingViewWhenTestIncomplete) {
    assertDoesNotHideTestingView(submittingPassedTrial);
}

TEST_F(
    PresenterTests, submitFailedTrialDoesNotHideTestingViewWhenTestIncomplete) {
    assertDoesNotHideTestingView(submittingFailedTrial);
}

TEST_F(PresenterTests, experimenterResponseShowsNextTrialButton) {
    assertShowsNextTrialButton(respondingFromExperimenter);
}

TEST_F(PresenterTests, subjectPassedTrialShowsNextTrialButtonForExperimenter) {
    assertShowsNextTrialButton(submittingPassedTrial);
}

TEST_F(PresenterTests, subjectFailedTrialShowsNextTrialButtonForExperimenter) {
    assertShowsNextTrialButton(submittingFailedTrial);
}

TEST_F(PresenterTests, experimenterResponseHidesResponseSubmission) {
    assertResponseViewHidden(respondingFromExperimenter);
}

TEST_F(PresenterTests, correctResponseHidesEvaluationButtons) {
    assertResponseViewHidden(submittingPassedTrial);
}

TEST_F(PresenterTests, incorrectResponseHidesEvaluationButtons) {
    assertResponseViewHidden(submittingFailedTrial);
}

TEST_F(PresenterTests, subjectResponseHidesResponseButtons) {
    assertResponseViewHidden(respondingFromSubject);
}

TEST_F(PresenterTests, subjectResponseHidesSubjectViewWhenTestComplete) {
    setTestComplete();
    respondFromSubject();
    assertSubjectViewHidden();
}

TEST_F(PresenterTests, exitTestHidesSubjectView) {
    exitTest();
    assertSubjectViewHidden();
}

TEST_F(PresenterTests, exitTestHidesExperimenterView) {
    assertHidesExperimenterView(exitingTest);
}

TEST_F(PresenterTests, exitTestHidesTestingView) {
    assertHidesTestingView(exitingTest);
}

TEST_F(PresenterTests, exitTestHidesResponseButtons) {
    run(exitingTest);
    assertTrue(respondingFromSubject.responseViewHidden());
}

TEST_F(PresenterTests, exitTestShowsTestSetupView) {
    exitTest();
    assertSetupViewShown();
}

TEST_F(PresenterTests, browseForTrackSettingsFileUpdatesTrackSettingsFile) {
    assertBrowseResultPassedToEntry(browsingForTrackSettingsFile);
}

TEST_F(PresenterTests, browseForTargetListUpdatesTargetList) {
    assertBrowseResultPassedToEntry(browsingForTargetList);
}

TEST_F(PresenterTests, browseForMaskerUpdatesMasker) {
    assertBrowseResultPassedToEntry(browsingForMasker);
}

TEST_F(PresenterTests, browseForCalibrationUpdatesCalibrationFilePaths) {
    assertBrowseResultPassedToEntry(browsingForCalibration);
}

TEST_F(PresenterTests, browseForTargetListCancelDoesNotChangeTargetList) {
    assertCancellingBrowseDoesNotChangePath(browsingForTargetList);
}

TEST_F(PresenterTests, browseForMaskerCancelDoesNotChangeMasker) {
    assertCancellingBrowseDoesNotChangePath(browsingForMasker);
}

TEST_F(PresenterTests,
    browseForCalibrationCancelDoesNotChangeCalibrationFilePath) {
    assertCancellingBrowseDoesNotChangePath(browsingForCalibration);
}

TEST_F(PresenterTests,
    browseForTrackSettingsFileCancelDoesNotChangeTrackSettingsFile) {
    assertCancellingBrowseDoesNotChangePath(browsingForTrackSettingsFile);
}

TEST_F(PresenterTests, completingTrialShowsExitTestButton) {
    completeTrial();
    assertTrue(exitTestButtonShown());
}

TEST_F(PresenterTests, confirmFixedLevelOpenSetTestShowsTrialNumber) {
    assertShowsTrialNumber(confirmingFixedLevelOpenSetTest);
}

TEST_F(PresenterTests, confirmAdaptiveOpenSetTestShowsTrialNumber) {
    assertShowsTrialNumber(confirmingAdaptiveOpenSetTest);
}

TEST_F(PresenterTests, confirmFixedLevelClosedSetTestShowsTrialNumber) {
    assertShowsTrialNumber(confirmingFixedLevelClosedSetTest);
}

TEST_F(PresenterTests, confirmAdaptiveClosedSetTestShowsTrialNumber) {
    assertShowsTrialNumber(confirmingAdaptiveClosedSetTest);
}

TEST_F(PresenterTests, submittingResponseFromExperimenterShowsTrialNumber) {
    assertShowsTrialNumber(respondingFromExperimenter);
}

TEST_F(PresenterTests, submittingResponseFromSubjectShowsTrialNumber) {
    assertShowsTrialNumber(respondingFromSubject);
}

TEST_F(PresenterTests, submittingPassedTrialShowsTrialNumber) {
    assertShowsTrialNumber(submittingPassedTrial);
}

TEST_F(PresenterTests, submittingFailedTrialShowsTrialNumber) {
    assertShowsTrialNumber(submittingFailedTrial);
}

TEST_F(PresenterTests,
    completingTrialShowsSubjectResponseButtonsForAdaptiveClosedSetTest) {
    assertCompleteTrialShowsResponseView(
        confirmingAdaptiveClosedSetTest, respondingFromSubject);
}

TEST_F(PresenterTests,
    completingTrialShowsSubjectResponseButtonsForAdaptiveClosedSetSingleSpeakerTest) {
    assertCompleteTrialShowsResponseView(
        confirmingAdaptiveClosedSetSingleSpeakerTest, respondingFromSubject);
}

TEST_F(PresenterTests,
    completingTrialShowsSubjectResponseButtonsForAdaptiveClosedSetDelayedMaskerTest) {
    assertCompleteTrialShowsResponseView(
        confirmingAdaptiveClosedSetDelayedMaskerTest, respondingFromSubject);
}

TEST_F(PresenterTests,
    completingTrialShowsSubjectResponseButtonsForFixedLevelClosedSetTest) {
    assertCompleteTrialShowsResponseView(
        confirmingFixedLevelClosedSetTest, respondingFromSubject);
}

TEST_F(PresenterTests,
    completingTrialShowsExperimenterEvaluationButtonsForAdaptiveOpenSetTest) {
    assertCompleteTrialShowsResponseView(
        confirmingAdaptiveOpenSetTest, submittingPassedTrial);
}

TEST_F(PresenterTests,
    completingTrialShowsExperimenterResponseSubmissionForFixedLevelOpenSetTest) {
    assertCompleteTrialShowsResponseView(
        confirmingFixedLevelOpenSetTest, respondingFromExperimenter);
}

TEST_F(PresenterTests,
    confirmAdaptiveClosedSetTestWithInvalidSnrShowsErrorMessage) {
    assertInvalidSnrShowsErrorMessage(confirmingAdaptiveClosedSetTest);
}

TEST_F(
    PresenterTests, confirmAdaptiveOpenSetTestWithInvalidSnrShowsErrorMessage) {
    assertInvalidSnrShowsErrorMessage(confirmingAdaptiveOpenSetTest);
}

TEST_F(PresenterTests,
    confirmFixedLevelOpenSetTestWithInvalidSnrShowsErrorMessage) {
    assertInvalidSnrShowsErrorMessage(confirmingFixedLevelOpenSetTest);
}

TEST_F(PresenterTests,
    confirmFixedLevelClosedSetTestWithInvalidSnrShowsErrorMessage) {
    assertInvalidSnrShowsErrorMessage(confirmingFixedLevelClosedSetTest);
}

TEST_F(PresenterTests,
    confirmAdaptiveClosedSetTestWithInvalidInputDoesNotHideSetupView) {
    assertSetupViewNotHiddenWhenSnrIsInvalid(confirmingAdaptiveClosedSetTest);
}

TEST_F(PresenterTests,
    confirmAdaptiveOpenSetTestWithInvalidInputDoesNotHideSetupView) {
    assertSetupViewNotHiddenWhenSnrIsInvalid(confirmingAdaptiveOpenSetTest);
}

TEST_F(PresenterTests,
    confirmFixedLevelOpenSetTestWithInvalidInputDoesNotHideSetupView) {
    assertSetupViewNotHiddenWhenSnrIsInvalid(confirmingFixedLevelOpenSetTest);
}

TEST_F(PresenterTests,
    confirmFixedLevelClosedSetTestWithInvalidInputDoesNotHideSetupView) {
    assertSetupViewNotHiddenWhenSnrIsInvalid(confirmingFixedLevelClosedSetTest);
}

TEST_F(PresenterFailureTests,
    initializeTestShowsErrorMessageWhenModelFailsRequest) {
    useFailingModel("a");
    assertConfirmTestSetupShowsErrorMessage("a");
}

TEST_F(PresenterFailureTests,
    initializeTestDoesNotHideSetupViewWhenModelFailsRequest) {
    useFailingModel();
    assertConfirmTestSetupDoesNotHideSetupView();
}
}
