#include "PresenterTests.hpp"

namespace av_speech_in_noise {
TEST_F(PresenterConstructionTests, populatesAudioDeviceMenu) {
    model.setAudioDevices({"a", "b", "c"});
    construct();
    assertEqual({"a", "b", "c"}, view.audioDevices());
}

#define PRESENTER_TEST(a) TEST_F(PresenterTests, a)

TEST_F(PresenterTests, confirmAdaptiveOpenSetKeywordsTestHidesTestSetupView) {
    assertHidesTestSetupView(confirmingAdaptiveOpenSetKeywordsTest);
}

TEST_F(
    PresenterTests, confirmAdaptiveOpenSetKeywordsTestShowsExperimenterView) {
    assertShowsExperimenterView(confirmingAdaptiveOpenSetKeywordsTest);
}

TEST_F(PresenterTests, confirmAdaptiveOpenSetKeywordsTestShowsTestingView) {
    assertShowsTestingView(confirmingAdaptiveOpenSetKeywordsTest);
}

TEST_F(
    PresenterTests, confirmAdaptiveOpenSetKeywordsTestDoesNotShowSubjectView) {
    assertDoesNotShowSubjectView(confirmingAdaptiveOpenSetKeywordsTest);
}

TEST_F(PresenterTests,
    confirmAdaptiveOpenSetKeywordsTestDoesNotInitializeFixedLevelTest) {
    assertDoesNotInitializeFixedLevelTest(
        confirmingAdaptiveOpenSetKeywordsTest);
}

TEST_F(PresenterTests, confirmingAdaptiveOpenSetKeywordsTestPassesStartingSnr) {
    assertStartingSnrPassedToModel(confirmingAdaptiveOpenSetKeywordsTest);
}

TEST_F(PresenterTests, confirmingAdaptiveOpenSetKeywordsTestPassesMaskerLevel) {
    assertMaskerLevelPassedToModel(confirmingAdaptiveOpenSetKeywordsTest);
}

TEST_F(PresenterTests,
    confirmAdaptiveOpenSetKeywordsTestShowsNextTrialButtonForExperimenter) {
    assertConfirmTestSetupShowsNextTrialButton(
        confirmingAdaptiveOpenSetKeywordsTest, playingTrialFromExperimenter);
}

TEST_F(PresenterTests, enteringCorrectKeywordsPassesCorrectKeywords) {
    setCorrectKeywords("1");
    run(enteringCorrectKeywords);
    assertEqual(1, model.correctKeywords());
}

TEST_F(PresenterTests, enteringInvalidCorrectKeywordsShowsErrorMessage) {
    setCorrectKeywords("a");
    run(enteringCorrectKeywords);
    assertErrorMessageEquals("'a' is not a valid number.");
}

TEST_F(PresenterTests, enteringInvalidCorrectKeywordsDoesNotHideEntry) {
    setCorrectKeywords("a");
    run(enteringCorrectKeywords);
    assertFalse(enteringCorrectKeywords.responseViewHidden());
}

TEST_F(PresenterTests, enteringCorrectKeywordsShowsSetupViewWhenTestComplete) {
    assertCompleteTestShowsSetupView(enteringCorrectKeywords);
}

TEST_F(PresenterTests,
    enteringCorrectKeywordsDoesNotShowSetupViewWhenTestIncomplete) {
    assertIncompleteTestDoesNotShowSetupView(enteringCorrectKeywords);
}

TEST_F(PresenterTests,
    enteringCorrectKeywordsHidesExperimenterViewWhenTestComplete) {
    assertCompleteTestHidesExperimenterView(enteringCorrectKeywords);
}

TEST_F(
    PresenterTests, enteringCorrectKeywordsHidesTestingViewWhenTestComplete) {
    assertCompleteTestHidesTestingView(enteringCorrectKeywords);
}

TEST_F(PresenterTests,
    enteringCorrectKeywordsDoesNotHideExperimenterViewWhenTestIncomplete) {
    assertDoesNotHideExperimenterView(enteringCorrectKeywords);
}

TEST_F(PresenterTests,
    enteringCorrectKeywordsDoesNotHideTestingViewWhenTestIncomplete) {
    assertDoesNotHideTestingView(enteringCorrectKeywords);
}

TEST_F(PresenterTests, enteringCorrectKeywordsShowsNextTrialButton) {
    assertShowsNextTrialButton(enteringCorrectKeywords);
}

TEST_F(PresenterTests, enteringCorrectKeywordsHidesCorrectKeywordsEntry) {
    assertResponseViewHidden(enteringCorrectKeywords);
}

TEST_F(PresenterTests, confirmAdaptiveOpenSetKeywordsTestShowsTrialNumber) {
    assertShowsTrialNumber(confirmingAdaptiveOpenSetKeywordsTest);
}

TEST_F(PresenterTests, enteringCorrectKeywordsShowsTrialNumber) {
    assertShowsTrialNumber(enteringCorrectKeywords);
}

TEST_F(PresenterTests,
    completingTrialShowsExperimenterCorrectKeywordsEntryForAdaptiveOpenSetKeywordsTest) {
    assertCompleteTrialShowsResponseView(
        confirmingAdaptiveOpenSetKeywordsTest, enteringCorrectKeywords);
}

TEST_F(PresenterTests,
    confirmAdaptiveOpenSetKeywordsTestWithInvalidSnrShowsErrorMessage) {
    assertInvalidSnrShowsErrorMessage(confirmingAdaptiveOpenSetKeywordsTest);
}

TEST_F(PresenterTests,
    confirmAdaptiveOpenSetKeywordsTestWithInvalidInputDoesNotHideSetupView) {
    assertSetupViewNotHiddenWhenSnrIsInvalid(
        confirmingAdaptiveOpenSetKeywordsTest);
}

PRESENTER_TEST(populatesConditionMenu) {
    assertSetupViewConditionsContains(auditoryOnlyConditionName());
    assertSetupViewConditionsContains(audioVisualConditionName());
}

PRESENTER_TEST(populatesMethodMenu) {
    assertSetupViewMethodsContains(Method::adaptivePassFail);
    assertSetupViewMethodsContains(Method::adaptiveCorrectKeywords);
    assertSetupViewMethodsContains(Method::defaultAdaptiveClosedSet);
    assertSetupViewMethodsContains(Method::adaptiveClosedSetWithSingleSpeaker);
    assertSetupViewMethodsContains(Method::adaptiveClosedSetWithDelayedMasker);
    assertSetupViewMethodsContains(Method::adaptiveClosedSetWithEyeTracking);
    assertSetupViewMethodsContains(Method::defaultFixedLevelOpenSet);
    assertSetupViewMethodsContains(Method::fixedLevelOpenSetWithAllTargets);
    assertSetupViewMethodsContains(
        Method::fixedLevelOpenSetWithSilentIntervalTargets);
    assertSetupViewMethodsContains(Method::defaultFixedLevelClosedSet);
    assertSetupViewMethodsContains(
        Method::fixedLevelClosedSetWithSilentIntervalTargets);
}

PRESENTER_TEST(callsEventLoopWhenRun) {
    presenter.run();
    assertTrue(view.eventLoopCalled());
}

PRESENTER_TEST(confirmDefaultAdaptiveClosedSetTestHidesTestSetupView) {
    assertHidesTestSetupView(confirmingDefaultAdaptiveClosedSetTest);
}

PRESENTER_TEST(
    confirmAdaptiveClosedSetTestWithSingleSpeakerHidesTestSetupView) {
    assertHidesTestSetupView(confirmingAdaptiveClosedSetTestWithSingleSpeaker);
}

PRESENTER_TEST(
    confirmAdaptiveClosedSetTestWithDelayedMaskerHidesTestSetupView) {
    assertHidesTestSetupView(confirmingAdaptiveClosedSetTestWithDelayedMasker);
}

PRESENTER_TEST(
    confirmAdaptiveClosedSetTestWithEyeTrackingHidesTestSetupView) {
    assertHidesTestSetupView(confirmingAdaptiveClosedSetTestWithEyeTracking);
}

PRESENTER_TEST(confirmAdaptiveOpenSetTestHidesTestSetupView) {
    assertHidesTestSetupView(confirmingAdaptiveOpenSetTest);
}

PRESENTER_TEST(confirmFixedLevelOpenSetTestHidesTestSetupView) {
    assertHidesTestSetupView(confirmingDefaultFixedLevelOpenSetTest);
}

PRESENTER_TEST(confirmFixedLevelClosedSetTestHidesTestSetupView) {
    assertHidesTestSetupView(confirmingDefaultFixedLevelClosedSetTest);
}

PRESENTER_TEST(
    confirmFixedLevelClosedSetTestWithSilentIntervalTargetsHidesTestSetupView) {
    assertHidesTestSetupView(confirmingFixedLevelClosedSetSilentIntervalsTest);
}

PRESENTER_TEST(confirmAdaptiveOpenSetTestShowsExperimenterView) {
    assertShowsExperimenterView(confirmingAdaptiveOpenSetTest);
}

PRESENTER_TEST(confirmAdaptiveOpenSetTestShowsTestingView) {
    assertShowsTestingView(confirmingAdaptiveOpenSetTest);
}

PRESENTER_TEST(confirmFixedLevelOpenSetTestShowsTestingView) {
    assertShowsTestingView(confirmingDefaultFixedLevelOpenSetTest);
}

PRESENTER_TEST(confirmFixedLevelOpenSetTestShowsExperimenterView) {
    assertShowsExperimenterView(confirmingDefaultFixedLevelOpenSetTest);
}

PRESENTER_TEST(confirmDefaultAdaptiveClosedSetTestShowsSubjectView) {
    assertShowsSubjectView(confirmingDefaultAdaptiveClosedSetTest);
}

PRESENTER_TEST(confirmAdaptiveClosedSetTestWithSingleSpeakerShowsSubjectView) {
    assertShowsSubjectView(confirmingAdaptiveClosedSetTestWithSingleSpeaker);
}

PRESENTER_TEST(confirmAdaptiveClosedSetTestWithDelayedMaskerShowsSubjectView) {
    assertShowsSubjectView(confirmingAdaptiveClosedSetTestWithDelayedMasker);
}

PRESENTER_TEST(confirmAdaptiveClosedSetTestWithEyeTrackingShowsSubjectView) {
    assertShowsSubjectView(confirmingAdaptiveClosedSetTestWithEyeTracking);
}

PRESENTER_TEST(
    confirmDefaultAdaptiveClosedSetTestDoesNotShowSubjectViewWhenTestComplete) {
    setTestComplete();
    assertDoesNotShowSubjectView(confirmingDefaultAdaptiveClosedSetTest);
}

PRESENTER_TEST(
    confirmDefaultAdaptiveClosedSetTestDoesNotHideSetupViewWhenTestComplete) {
    setTestComplete();
    assertDoesNotHideTestSetupView(confirmingDefaultAdaptiveClosedSetTest);
}

PRESENTER_TEST(confirmFixedLevelClosedSetTestShowsSubjectView) {
    assertShowsSubjectView(confirmingDefaultFixedLevelClosedSetTest);
}

PRESENTER_TEST(
    confirmFixedLevelClosedSetTestWithSilentIntervalTargetsShowsSubjectView) {
    assertShowsSubjectView(confirmingFixedLevelClosedSetSilentIntervalsTest);
}

PRESENTER_TEST(confirmAdaptiveOpenSetTestDoesNotShowSubjectView) {
    assertDoesNotShowSubjectView(confirmingAdaptiveOpenSetTest);
}

PRESENTER_TEST(confirmFixedLevelOpenSetTestDoesNotShowSubjectView) {
    assertDoesNotShowSubjectView(confirmingDefaultFixedLevelOpenSetTest);
}

PRESENTER_TEST(
    confirmFixedLevelClosedSetTestWithAllTargetsDoesNotShowSubjectView) {
    assertDoesNotShowSubjectView(confirmingFixedLevelOpenSetTestWithAllTargets);
}

PRESENTER_TEST(
    confirmFixedLevelOpenSetTestWithSilentIntervalTargetsDoesNotShowSubjectView) {
    assertDoesNotShowSubjectView(
        confirmingFixedLevelOpenSetSilentIntervalsTest);
}

PRESENTER_TEST(confirmDefaultAdaptiveClosedSetTestShowsExperimenterView) {
    assertShowsExperimenterView(confirmingDefaultAdaptiveClosedSetTest);
}

PRESENTER_TEST(confirmFixedLevelClosedSetTestShowsExperimenterView) {
    assertShowsExperimenterView(confirmingDefaultFixedLevelClosedSetTest);
}

PRESENTER_TEST(
    confirmFixedLevelClosedSetTestWithSilentIntervalTargetsShowsExperimenterView) {
    assertShowsExperimenterView(
        confirmingFixedLevelClosedSetSilentIntervalsTest);
}

PRESENTER_TEST(confirmAdaptiveClosedSetTestWithSingleSpeakerInitializesModel) {
    run(confirmingAdaptiveClosedSetTestWithSingleSpeaker);
    assertTrue(model.initializedWithSingleSpeaker());
}

PRESENTER_TEST(confirmAdaptiveClosedSetTestWithDelayedMaskerInitializesModel) {
    run(confirmingAdaptiveClosedSetTestWithDelayedMasker);
    assertTrue(model.initializedWithDelayedMasker());
}

PRESENTER_TEST(confirmAdaptiveClosedSetTestWithEyeTrackingInitializesModel) {
    run(confirmingAdaptiveClosedSetTestWithEyeTracking);
    assertTrue(model.initializedWithEyeTracking());
}

PRESENTER_TEST(
    confirmFixedLevelClosedSetTestWithSilentIntervalTargetsInitializesModel) {
    run(confirmingFixedLevelClosedSetSilentIntervalsTest);
    assertTrue(model.fixedLevelTestWithSilentIntervalTargetsInitialized());
}

PRESENTER_TEST(confirmFixedLevelClosedSetTestWithAllTargetsInitializesModel) {
    run(confirmingFixedLevelOpenSetTestWithAllTargets);
    assertTrue(model.fixedLevelTestWithAllTargetsInitialized());
}

PRESENTER_TEST(
    confirmFixedLevelOpenSetTestWithSilentIntervalTargetsInitializesModel) {
    run(confirmingFixedLevelOpenSetSilentIntervalsTest);
    assertTrue(model.fixedLevelTestWithSilentIntervalTargetsInitialized());
}

PRESENTER_TEST(confirmDefaultAdaptiveClosedSetTestDoesNotInitializeFixedLevelTest) {
    assertDoesNotInitializeFixedLevelTest(
        confirmingDefaultAdaptiveClosedSetTest);
}

PRESENTER_TEST(confirmAdaptiveOpenSetTestDoesNotInitializeFixedLevelTest) {
    assertDoesNotInitializeFixedLevelTest(confirmingAdaptiveOpenSetTest);
}

PRESENTER_TEST(confirmFixedLevelOpenSetTestDoesNotInitializeAdaptiveTest) {
    assertDoesNotInitializeAdaptiveTest(confirmingDefaultFixedLevelOpenSetTest);
}

PRESENTER_TEST(
    confirmFixedLevelClosedSetTestWithAllTargetsDoesNotInitializeAdaptiveTest) {
    assertDoesNotInitializeAdaptiveTest(
        confirmingFixedLevelOpenSetTestWithAllTargets);
}

PRESENTER_TEST(
    confirmFixedLevelOpenSetTestWithSilentIntervalTargetsDoesNotInitializeAdaptiveTest) {
    assertDoesNotInitializeAdaptiveTest(
        confirmingFixedLevelOpenSetSilentIntervalsTest);
}

PRESENTER_TEST(confirmFixedLevelClosedSetTestDoesNotInitializeAdaptiveTest) {
    assertDoesNotInitializeAdaptiveTest(
        confirmingDefaultFixedLevelClosedSetTest);
}

PRESENTER_TEST(
    confirmFixedLevelClosedSetTestWithSilentIntervalTargetsDoesNotInitializeAdaptiveTest) {
    assertDoesNotInitializeAdaptiveTest(
        confirmingFixedLevelClosedSetSilentIntervalsTest);
}

PRESENTER_TEST(confirmAdaptiveClosedSetTestPassesStartingSnr) {
    assertStartingSnrPassedToModel(confirmingDefaultAdaptiveClosedSetTest);
}

PRESENTER_TEST(confirmAdaptiveOpenSetTestPassesStartingSnr) {
    assertStartingSnrPassedToModel(confirmingAdaptiveOpenSetTest);
}

PRESENTER_TEST(confirmFixedLevelOpenSetTestPassesStartingSnr) {
    assertStartingSnrPassedToModel(confirmingDefaultFixedLevelOpenSetTest);
}

PRESENTER_TEST(confirmFixedLevelClosedSetTestPassesStartingSnr) {
    assertStartingSnrPassedToModel(confirmingDefaultFixedLevelClosedSetTest);
}

PRESENTER_TEST(
    confirmFixedLevelClosedSetTestWithSilentIntervalTargetsPassesStartingSnr) {
    assertStartingSnrPassedToModel(
        confirmingFixedLevelClosedSetSilentIntervalsTest);
}

PRESENTER_TEST(confirmAdaptiveClosedSetTestPassesMaskerLevel) {
    assertMaskerLevelPassedToModel(confirmingDefaultAdaptiveClosedSetTest);
}

PRESENTER_TEST(confirmAdaptiveOpenSetTestPassesMaskerLevel) {
    assertMaskerLevelPassedToModel(confirmingAdaptiveOpenSetTest);
}

PRESENTER_TEST(confirmFixedLevelOpenSetTestPassesMaskerLevel) {
    assertMaskerLevelPassedToModel(confirmingDefaultFixedLevelOpenSetTest);
}

PRESENTER_TEST(confirmFixedLevelClosedSetTestPassesMaskerLevel) {
    assertMaskerLevelPassedToModel(confirmingDefaultFixedLevelClosedSetTest);
}

PRESENTER_TEST(playCalibrationPassesLevel) {
    setCalibrationLevel("1");
    playCalibration();
    assertEqual(1, calibration().level_dB_SPL);
}

PRESENTER_TEST(confirmAdaptiveClosedSetTestPassesTargetList) {
    assertPassesTargetListDirectory(confirmingDefaultAdaptiveClosedSetTest);
}

PRESENTER_TEST(confirmAdaptiveOpenSetTestPassesTargetList) {
    assertPassesTargetListDirectory(confirmingAdaptiveOpenSetTest);
}

PRESENTER_TEST(confirmFixedLevelOpenSetTestPassesTargetList) {
    assertPassesTargetListDirectory(confirmingDefaultFixedLevelOpenSetTest);
}

PRESENTER_TEST(confirmFixedLevelClosedSetTestPassesTargetList) {
    assertPassesTargetListDirectory(confirmingDefaultFixedLevelClosedSetTest);
}

PRESENTER_TEST(confirmAdaptiveClosedSetTestPassesSubjectId) {
    assertPassesSubjectId(confirmingDefaultAdaptiveClosedSetTest);
}

PRESENTER_TEST(confirmAdaptiveOpenSetTestPassesSubjectId) {
    assertPassesSubjectId(confirmingAdaptiveOpenSetTest);
}

PRESENTER_TEST(confirmFixedLevelOpenSetTestPassesSubjectId) {
    assertPassesSubjectId(confirmingDefaultFixedLevelOpenSetTest);
}

PRESENTER_TEST(confirmFixedLevelClosedSetTestPassesSubjectId) {
    assertPassesSubjectId(confirmingDefaultFixedLevelClosedSetTest);
}

PRESENTER_TEST(confirmAdaptiveClosedSetTestPassesTesterId) {
    assertPassesTesterId(confirmingDefaultAdaptiveClosedSetTest);
}

PRESENTER_TEST(confirmAdaptiveOpenSetTestPassesTesterId) {
    assertPassesTesterId(confirmingAdaptiveOpenSetTest);
}

PRESENTER_TEST(confirmFixedLevelOpenSetTestPassesTesterId) {
    assertPassesTesterId(confirmingDefaultFixedLevelOpenSetTest);
}

PRESENTER_TEST(confirmFixedLevelClosedSetTestPassesTesterId) {
    assertPassesTesterId(confirmingDefaultFixedLevelClosedSetTest);
}

PRESENTER_TEST(confirmAdaptiveClosedSetTestPassesMasker) {
    assertPassesMasker(confirmingDefaultAdaptiveClosedSetTest);
}

PRESENTER_TEST(confirmAdaptiveOpenSetTestPassesMasker) {
    assertPassesMasker(confirmingAdaptiveOpenSetTest);
}

PRESENTER_TEST(confirmFixedLevelOpenSetTestPassesMasker) {
    assertPassesMasker(confirmingDefaultFixedLevelOpenSetTest);
}

PRESENTER_TEST(confirmFixedLevelClosedSetTestPassesMasker) {
    assertPassesMasker(confirmingDefaultFixedLevelClosedSetTest);
}

PRESENTER_TEST(playCalibrationPassesFilePath) {
    setupView.setCalibrationFilePath("a");
    playCalibration();
    assertEqual("a", calibration().filePath);
}

PRESENTER_TEST(confirmAdaptiveClosedSetTestPassesSession) {
    assertPassesSession(confirmingDefaultAdaptiveClosedSetTest);
}

PRESENTER_TEST(confirmAdaptiveOpenSetTestPassesSession) {
    assertPassesSession(confirmingAdaptiveOpenSetTest);
}

PRESENTER_TEST(confirmFixedLevelOpenSetTestPassesSession) {
    assertPassesSession(confirmingDefaultFixedLevelOpenSetTest);
}

PRESENTER_TEST(confirmFixedLevelClosedSetTestPassesSession) {
    assertPassesSession(confirmingDefaultFixedLevelClosedSetTest);
}

PRESENTER_TEST(confirmAdaptiveClosedSetTestPassesMethod) {
    assertPassesMethod(confirmingDefaultAdaptiveClosedSetTest);
}

PRESENTER_TEST(confirmAdaptiveOpenSetTestPassesMethod) {
    assertPassesMethod(confirmingAdaptiveOpenSetTest);
}

PRESENTER_TEST(confirmFixedLevelOpenSetTestPassesMethod) {
    assertPassesMethod(confirmingDefaultFixedLevelOpenSetTest);
}

PRESENTER_TEST(confirmFixedLevelClosedSetTestPassesMethod) {
    assertPassesMethod(confirmingDefaultFixedLevelClosedSetTest);
}

PRESENTER_TEST(confirmAdaptiveClosedSetTestWithDelayedMaskerPassesMethod) {
    assertPassesMethod(confirmingAdaptiveClosedSetTestWithDelayedMasker);
}

PRESENTER_TEST(confirmAdaptiveClosedSetTestWithEyeTrackingPassesMethod) {
    assertPassesMethod(confirmingAdaptiveClosedSetTestWithEyeTracking);
}

PRESENTER_TEST(confirmAdaptiveClosedSetSingleSpeakerTestPassesMethod) {
    assertPassesMethod(confirmingAdaptiveClosedSetTestWithSingleSpeaker);
}

PRESENTER_TEST(confirmAdaptiveClosedSetTestPassesCeilingSNR) {
    assertPassesCeilingSNR(confirmingDefaultAdaptiveClosedSetTest);
}

PRESENTER_TEST(confirmAdaptiveOpenSetTestPassesCeilingSNR) {
    assertPassesCeilingSNR(confirmingAdaptiveOpenSetTest);
}

PRESENTER_TEST(confirmAdaptiveClosedSetTestPassesFloorSNR) {
    assertPassesFloorSNR(confirmingDefaultAdaptiveClosedSetTest);
}

PRESENTER_TEST(confirmAdaptiveOpenSetTestPassesFloorSNR) {
    assertPassesFloorSNR(confirmingAdaptiveOpenSetTest);
}

PRESENTER_TEST(confirmAdaptiveClosedSetTestPassesTrackBumpLimit) {
    assertPassesTrackBumpLimit(confirmingDefaultAdaptiveClosedSetTest);
}

PRESENTER_TEST(confirmAdaptiveOpenSetTestPassesTrackBumpLimit) {
    assertPassesTrackBumpLimit(confirmingAdaptiveOpenSetTest);
}

PRESENTER_TEST(confirmAdaptiveClosedSetTestPassesFullScaleLevel) {
    assertPassesFullScaleLevel(confirmingDefaultAdaptiveClosedSetTest);
}

PRESENTER_TEST(confirmAdaptiveOpenSetTestPassesFullScaleLevel) {
    assertPassesFullScaleLevel(confirmingAdaptiveOpenSetTest);
}

PRESENTER_TEST(confirmFixedLevelOpenSetTestPassesFullScaleLevel) {
    assertPassesFullScaleLevel(confirmingDefaultFixedLevelOpenSetTest);
}

PRESENTER_TEST(confirmFixedLevelClosedSetTestPassesFullScaleLevel) {
    assertPassesFullScaleLevel(confirmingDefaultFixedLevelClosedSetTest);
}

PRESENTER_TEST(playCalibrationPassesFullScaleLevel) {
    assertPassesFullScaleLevel(playingCalibration);
}

PRESENTER_TEST(confirmAdaptiveClosedSetTestPassesTrackSettingsFile) {
    assertPassesTrackSettingsFile(confirmingDefaultAdaptiveClosedSetTest);
}

PRESENTER_TEST(confirmAdaptiveOpenSetTestPassesTrackSettingsFile) {
    assertPassesTrackSettingsFile(confirmingAdaptiveOpenSetTest);
}

PRESENTER_TEST(confirmAdaptiveClosedSetTestPassesAudioVisualCondition) {
    assertAudioVisualConditionPassedToModel(
        confirmingDefaultAdaptiveClosedSetTest);
}

PRESENTER_TEST(confirmAdaptiveClosedSetTestPassesAuditoryOnlyCondition) {
    assertAuditoryOnlyConditionPassedToModel(
        confirmingDefaultAdaptiveClosedSetTest);
}

PRESENTER_TEST(confirmAdaptiveOpenSetTestPassesAudioVisualCondition) {
    assertAudioVisualConditionPassedToModel(confirmingAdaptiveOpenSetTest);
}

PRESENTER_TEST(confirmAdaptiveOpenSetTestPassesAuditoryOnlyCondition) {
    assertAuditoryOnlyConditionPassedToModel(confirmingAdaptiveOpenSetTest);
}

PRESENTER_TEST(confirmFixedLevelOpenSetTestPassesAudioVisualCondition) {
    assertAudioVisualConditionPassedToModel(
        confirmingDefaultFixedLevelOpenSetTest);
}

PRESENTER_TEST(confirmFixedLevelOpenSetTestPassesAuditoryOnlyCondition) {
    assertAuditoryOnlyConditionPassedToModel(
        confirmingDefaultFixedLevelOpenSetTest);
}

PRESENTER_TEST(confirmFixedLevelClosedSetTestPassesAudioVisualCondition) {
    assertAudioVisualConditionPassedToModel(
        confirmingDefaultFixedLevelClosedSetTest);
}

PRESENTER_TEST(confirmFixedLevelClosedetTestPassesAuditoryOnlyCondition) {
    assertAuditoryOnlyConditionPassedToModel(
        confirmingDefaultFixedLevelClosedSetTest);
}

PRESENTER_TEST(playCalibrationPassesAudioVisualCondition) {
    assertAudioVisualConditionPassedToModel(playingCalibration);
}

PRESENTER_TEST(playCalibrationPassesAuditoryOnlyCondition) {
    assertAuditoryOnlyConditionPassedToModel(playingCalibration);
}

PRESENTER_TEST(confirmDefaultAdaptiveClosedSetTestShowsNextTrialButtonForSubject) {
    assertConfirmTestSetupShowsNextTrialButton(
        confirmingDefaultAdaptiveClosedSetTest, playingTrialFromSubject);
}

PRESENTER_TEST(confirmFixedLevelClosedSetTestShowsNextTrialButtonForSubject) {
    assertConfirmTestSetupShowsNextTrialButton(
        confirmingDefaultFixedLevelClosedSetTest, playingTrialFromSubject);
}

PRESENTER_TEST(
    confirmFixedLevelClosedSetTestWithSilentIntervalTargetsShowsNextTrialButtonForSubject) {
    assertConfirmTestSetupShowsNextTrialButton(
        confirmingFixedLevelClosedSetSilentIntervalsTest,
        playingTrialFromSubject);
}

PRESENTER_TEST(confirmAdaptiveOpenSetTestShowsNextTrialButtonForExperimenter) {
    assertConfirmTestSetupShowsNextTrialButton(
        confirmingAdaptiveOpenSetTest, playingTrialFromExperimenter);
}

PRESENTER_TEST(
    confirmFixedLevelOpenSetTestShowsNextTrialButtonForExperimenter) {
    assertConfirmTestSetupShowsNextTrialButton(
        confirmingDefaultFixedLevelOpenSetTest, playingTrialFromExperimenter);
}

PRESENTER_TEST(
    confirmFixedLevelClosedSetTestWithAllTargetsShowsNextTrialButtonForExperimenter) {
    assertConfirmTestSetupShowsNextTrialButton(
        confirmingFixedLevelOpenSetTestWithAllTargets,
        playingTrialFromExperimenter);
}

PRESENTER_TEST(
    confirmFixedLevelOpenSetTestWithSilentIntervalTargetsShowsNextTrialButtonForExperimenter) {
    assertConfirmTestSetupShowsNextTrialButton(
        confirmingFixedLevelOpenSetSilentIntervalsTest,
        playingTrialFromExperimenter);
}

PRESENTER_TEST(
    confirmingAdaptiveClosedSetTestWithInvalidMaskerLevelShowsErrorMessage) {
    assertInvalidMaskerLevelShowsErrorMessage(
        confirmingDefaultAdaptiveClosedSetTest);
}

PRESENTER_TEST(
    confirmingAdaptiveOpenSetTestWithInvalidMaskerLevelShowsErrorMessage) {
    assertInvalidMaskerLevelShowsErrorMessage(confirmingAdaptiveOpenSetTest);
}

PRESENTER_TEST(
    confirmingFixedLevelOpenSetTestWithInvalidMaskerLevelShowsErrorMessage) {
    assertInvalidMaskerLevelShowsErrorMessage(
        confirmingDefaultFixedLevelOpenSetTest);
}

PRESENTER_TEST(
    confirmingFixedLevelClosedSetTestWithInvalidMaskerLevelShowsErrorMessage) {
    assertInvalidMaskerLevelShowsErrorMessage(
        confirmingDefaultFixedLevelClosedSetTest);
}

PRESENTER_TEST(playCalibrationWithInvalidLevelShowsErrorMessage) {
    assertInvalidCalibrationLevelShowsErrorMessage(playingCalibration);
}

PRESENTER_TEST(respondingFromSubjectPlaysTrial) {
    assertPlaysTrial(respondingFromSubject);
}

PRESENTER_TEST(playingTrialFromSubjectPlaysTrial) {
    assertPlaysTrial(playingTrialFromSubject);
}

PRESENTER_TEST(playingTrialFromExperimenterPlaysTrial) {
    assertPlaysTrial(playingTrialFromExperimenter);
}

PRESENTER_TEST(playingTrialHidesNextTrialButton) {
    assertHidesPlayTrialButton(playingTrialFromSubject);
}

PRESENTER_TEST(playingTrialHidesNextTrialButtonForExperimenter) {
    assertHidesPlayTrialButton(playingTrialFromExperimenter);
}

PRESENTER_TEST(playingTrialFromSubjectHidesExitTestButton) {
    assertHidesExitTestButton(playingTrialFromSubject);
}

PRESENTER_TEST(playingTrialFromSubjectPassesAudioDevice) {
    assertAudioDevicePassedToTrial(playingTrialFromSubject);
}

PRESENTER_TEST(playingTrialFromExperimenterPassesAudioDevice) {
    assertAudioDevicePassedToTrial(playingTrialFromExperimenter);
}

PRESENTER_TEST(playCalibrationPassesAudioDevice) {
    setAudioDevice("b");
    playCalibration();
    assertEqual("b", calibration().audioSettings.audioDevice);
}

PRESENTER_TEST(subjectResponsePassesNumberResponse) {
    subjectView.setNumberResponse("1");
    respondFromSubject();
    assertEqual(1, model.responseParameters().number);
}

PRESENTER_TEST(subjectResponsePassesGreenColor) {
    subjectView.setGreenResponse();
    respondFromSubject();
    assertModelPassedCondition(coordinate_response_measure::Color::green);
}

PRESENTER_TEST(subjectResponsePassesRedColor) {
    subjectView.setRedResponse();
    respondFromSubject();
    assertModelPassedCondition(coordinate_response_measure::Color::red);
}

PRESENTER_TEST(subjectResponsePassesBlueColor) {
    subjectView.setBlueResponse();
    respondFromSubject();
    assertModelPassedCondition(coordinate_response_measure::Color::blue);
}

PRESENTER_TEST(subjectResponsePassesWhiteColor) {
    subjectView.setGrayResponse();
    respondFromSubject();
    assertModelPassedCondition(coordinate_response_measure::Color::white);
}

PRESENTER_TEST(experimenterResponsePassesResponse) {
    testingView.setResponse("a");
    respondFromExperimenter();
    assertEqual("a", model.freeResponse().response);
}

PRESENTER_TEST(experimenterResponseFlagsResponse) {
    testingView.flagResponse();
    respondFromExperimenter();
    assertTrue(model.freeResponse().flagged);
}

PRESENTER_TEST(passedTrialSubmitsCorrectResponse) {
    run(submittingPassedTrial);
    assertTrue(model.correctResponseSubmitted());
}

PRESENTER_TEST(failedTrialSubmitsIncorrectResponse) {
    run(submittingFailedTrial);
    assertTrue(model.incorrectResponseSubmitted());
}

PRESENTER_TEST(respondFromSubjectShowsSetupViewWhenTestComplete) {
    assertCompleteTestShowsSetupView(respondingFromSubject);
}

PRESENTER_TEST(respondFromExperimenterShowsSetupViewWhenTestComplete) {
    assertCompleteTestShowsSetupView(respondingFromExperimenter);
}

PRESENTER_TEST(submitPassedTrialShowsSetupViewWhenTestComplete) {
    assertCompleteTestShowsSetupView(submittingPassedTrial);
}

PRESENTER_TEST(submitFailedTrialShowsSetupViewWhenTestComplete) {
    assertCompleteTestShowsSetupView(submittingFailedTrial);
}

PRESENTER_TEST(respondFromSubjectDoesNotShowSetupViewWhenTestIncomplete) {
    assertIncompleteTestDoesNotShowSetupView(respondingFromSubject);
}

PRESENTER_TEST(respondFromExperimenterDoesNotShowSetupViewWhenTestIncomplete) {
    assertIncompleteTestDoesNotShowSetupView(respondingFromExperimenter);
}

PRESENTER_TEST(submitPassedTrialDoesNotShowSetupViewWhenTestIncomplete) {
    assertIncompleteTestDoesNotShowSetupView(submittingPassedTrial);
}

PRESENTER_TEST(submitFailedTrialDoesNotShowSetupViewWhenTestIncomplete) {
    assertIncompleteTestDoesNotShowSetupView(submittingFailedTrial);
}

PRESENTER_TEST(respondFromSubjectHidesExperimenterViewWhenTestComplete) {
    assertCompleteTestHidesExperimenterView(respondingFromSubject);
}

PRESENTER_TEST(respondFromExperimenterHidesExperimenterViewWhenTestComplete) {
    assertCompleteTestHidesExperimenterView(respondingFromExperimenter);
}

PRESENTER_TEST(submitPassedTrialHidesExperimenterViewWhenTestComplete) {
    assertCompleteTestHidesExperimenterView(submittingPassedTrial);
}

PRESENTER_TEST(submitFailedTrialHidesExperimenterViewWhenTestComplete) {
    assertCompleteTestHidesExperimenterView(submittingFailedTrial);
}

PRESENTER_TEST(respondFromExperimenterHidesTestingViewWhenTestComplete) {
    assertCompleteTestHidesTestingView(respondingFromExperimenter);
}

PRESENTER_TEST(submitPassedTrialHidesTestingViewWhenTestComplete) {
    assertCompleteTestHidesTestingView(submittingPassedTrial);
}

PRESENTER_TEST(submitFailedTrialHidesTestingViewWhenTestComplete) {
    assertCompleteTestHidesTestingView(submittingFailedTrial);
}

PRESENTER_TEST(submitCoordinateResponseDoesNotPlayTrialWhenTestComplete) {
    assertCompleteTestDoesNotPlayTrial(respondingFromSubject);
}

PRESENTER_TEST(
    respondFromSubjectDoesNotHideExperimenterViewWhenTestIncomplete) {
    assertDoesNotHideExperimenterView(respondingFromSubject);
}

PRESENTER_TEST(
    respondFromExperimenterDoesNotHideExperimenterViewWhenTestIncomplete) {
    assertDoesNotHideExperimenterView(respondingFromExperimenter);
}

PRESENTER_TEST(submitPassedTrialDoesNotHideExperimenterViewWhenTestIncomplete) {
    assertDoesNotHideExperimenterView(submittingPassedTrial);
}

PRESENTER_TEST(submitFailedTrialDoesNotHideExperimenterViewWhenTestIncomplete) {
    assertDoesNotHideExperimenterView(submittingFailedTrial);
}

PRESENTER_TEST(
    respondFromExperimenterDoesNotHideTestingViewWhenTestIncomplete) {
    assertDoesNotHideTestingView(respondingFromExperimenter);
}

PRESENTER_TEST(submitPassedTrialDoesNotHideTestingViewWhenTestIncomplete) {
    assertDoesNotHideTestingView(submittingPassedTrial);
}

PRESENTER_TEST(submitFailedTrialDoesNotHideTestingViewWhenTestIncomplete) {
    assertDoesNotHideTestingView(submittingFailedTrial);
}

PRESENTER_TEST(experimenterResponseShowsNextTrialButton) {
    assertShowsNextTrialButton(respondingFromExperimenter);
}

PRESENTER_TEST(subjectPassedTrialShowsNextTrialButtonForExperimenter) {
    assertShowsNextTrialButton(submittingPassedTrial);
}

PRESENTER_TEST(subjectFailedTrialShowsNextTrialButtonForExperimenter) {
    assertShowsNextTrialButton(submittingFailedTrial);
}

PRESENTER_TEST(experimenterResponseHidesResponseSubmission) {
    assertResponseViewHidden(respondingFromExperimenter);
}

PRESENTER_TEST(correctResponseHidesEvaluationButtons) {
    assertResponseViewHidden(submittingPassedTrial);
}

PRESENTER_TEST(incorrectResponseHidesEvaluationButtons) {
    assertResponseViewHidden(submittingFailedTrial);
}

PRESENTER_TEST(subjectResponseHidesResponseButtons) {
    assertResponseViewHidden(respondingFromSubject);
}

PRESENTER_TEST(subjectResponseHidesSubjectViewWhenTestComplete) {
    setTestComplete();
    respondFromSubject();
    assertSubjectViewHidden();
}

PRESENTER_TEST(exitTestHidesSubjectView) {
    exitTest();
    assertSubjectViewHidden();
}

PRESENTER_TEST(exitTestHidesExperimenterView) {
    assertHidesExperimenterView(exitingTest);
}

PRESENTER_TEST(exitTestHidesTestingView) {
    assertHidesTestingView(exitingTest);
}

PRESENTER_TEST(exitTestHidesResponseButtons) {
    run(exitingTest);
    assertTrue(respondingFromSubject.responseViewHidden());
}

PRESENTER_TEST(exitTestShowsTestSetupView) {
    exitTest();
    assertSetupViewShown();
}

PRESENTER_TEST(browseForTrackSettingsFileUpdatesTrackSettingsFile) {
    assertBrowseResultPassedToEntry(browsingForTrackSettingsFile);
}

PRESENTER_TEST(browseForTargetListUpdatesTargetList) {
    assertBrowseResultPassedToEntry(browsingForTargetList);
}

PRESENTER_TEST(browseForMaskerUpdatesMasker) {
    assertBrowseResultPassedToEntry(browsingForMasker);
}

PRESENTER_TEST(browseForCalibrationUpdatesCalibrationFilePaths) {
    assertBrowseResultPassedToEntry(browsingForCalibration);
}

PRESENTER_TEST(browseForTargetListCancelDoesNotChangeTargetList) {
    assertCancellingBrowseDoesNotChangePath(browsingForTargetList);
}

PRESENTER_TEST(browseForMaskerCancelDoesNotChangeMasker) {
    assertCancellingBrowseDoesNotChangePath(browsingForMasker);
}

PRESENTER_TEST(browseForCalibrationCancelDoesNotChangeCalibrationFilePath) {
    assertCancellingBrowseDoesNotChangePath(browsingForCalibration);
}

PRESENTER_TEST(browseForTrackSettingsFileCancelDoesNotChangeTrackSettingsFile) {
    assertCancellingBrowseDoesNotChangePath(browsingForTrackSettingsFile);
}

PRESENTER_TEST(completingTrialShowsExitTestButton) {
    completeTrial();
    assertTrue(exitTestButtonShown());
}

PRESENTER_TEST(confirmFixedLevelOpenSetTestShowsTrialNumber) {
    assertShowsTrialNumber(confirmingDefaultFixedLevelOpenSetTest);
}

PRESENTER_TEST(confirmFixedLevelClosedSetTestWithAllTargetsShowsTrialNumber) {
    assertShowsTrialNumber(confirmingFixedLevelOpenSetTestWithAllTargets);
}

PRESENTER_TEST(
    confirmFixedLevelOpenSetTestWithSilentIntervalTargetsShowsTrialNumber) {
    assertShowsTrialNumber(confirmingFixedLevelOpenSetSilentIntervalsTest);
}

PRESENTER_TEST(confirmAdaptiveOpenSetTestShowsTrialNumber) {
    assertShowsTrialNumber(confirmingAdaptiveOpenSetTest);
}

PRESENTER_TEST(confirmFixedLevelClosedSetTestShowsTrialNumber) {
    assertShowsTrialNumber(confirmingDefaultFixedLevelClosedSetTest);
}

PRESENTER_TEST(
    confirmFixedLevelClosedSetTestWithSilentIntervalTargetsShowsTrialNumber) {
    assertShowsTrialNumber(confirmingFixedLevelClosedSetSilentIntervalsTest);
}

PRESENTER_TEST(confirmDefaultAdaptiveClosedSetTestShowsTrialNumber) {
    assertShowsTrialNumber(confirmingDefaultAdaptiveClosedSetTest);
}

PRESENTER_TEST(submittingResponseFromExperimenterShowsTrialNumber) {
    assertShowsTrialNumber(respondingFromExperimenter);
}

PRESENTER_TEST(submittingResponseFromSubjectShowsTrialNumber) {
    assertShowsTrialNumber(respondingFromSubject);
}

PRESENTER_TEST(submittingPassedTrialShowsTrialNumber) {
    assertShowsTrialNumber(submittingPassedTrial);
}

PRESENTER_TEST(submittingFailedTrialShowsTrialNumber) {
    assertShowsTrialNumber(submittingFailedTrial);
}

PRESENTER_TEST(
    completingTrialShowsSubjectResponseButtonsForAdaptiveClosedSetTest) {
    assertCompleteTrialShowsResponseView(
        confirmingDefaultAdaptiveClosedSetTest, respondingFromSubject);
}

PRESENTER_TEST(
    completingTrialShowsSubjectResponseButtonsForAdaptiveClosedSetSingleSpeakerTest) {
    assertCompleteTrialShowsResponseView(
        confirmingAdaptiveClosedSetTestWithSingleSpeaker,
        respondingFromSubject);
}

PRESENTER_TEST(
    completingTrialShowsSubjectResponseButtonsForAdaptiveClosedSetTestWithDelayedMasker) {
    assertCompleteTrialShowsResponseView(
        confirmingAdaptiveClosedSetTestWithDelayedMasker,
        respondingFromSubject);
}

PRESENTER_TEST(
    completingTrialShowsSubjectResponseButtonsForAdaptiveClosedSetTestWithEyeTracking) {
    assertCompleteTrialShowsResponseView(
        confirmingAdaptiveClosedSetTestWithEyeTracking,
        respondingFromSubject);
}

PRESENTER_TEST(
    completingTrialShowsSubjectResponseButtonsForFixedLevelClosedSetTest) {
    assertCompleteTrialShowsResponseView(
        confirmingDefaultFixedLevelClosedSetTest, respondingFromSubject);
}

PRESENTER_TEST(
    completingTrialShowsSubjectResponseButtonsForFixedLevelClosedSetSilentIntervalsTest) {
    assertCompleteTrialShowsResponseView(
        confirmingFixedLevelClosedSetSilentIntervalsTest,
        respondingFromSubject);
}

PRESENTER_TEST(
    completingTrialShowsExperimenterEvaluationButtonsForAdaptiveOpenSetTest) {
    assertCompleteTrialShowsResponseView(
        confirmingAdaptiveOpenSetTest, submittingPassedTrial);
}

PRESENTER_TEST(
    completingTrialShowsExperimenterResponseSubmissionForFixedLevelOpenSetTest) {
    assertCompleteTrialShowsResponseView(
        confirmingDefaultFixedLevelOpenSetTest, respondingFromExperimenter);
}

PRESENTER_TEST(
    completingTrialShowsExperimenterResponseSubmissionForFixedLevelOpenSetAllStimuliTest) {
    assertCompleteTrialShowsResponseView(
        confirmingFixedLevelOpenSetTestWithAllTargets,
        respondingFromExperimenter);
}

PRESENTER_TEST(
    completingTrialShowsExperimenterResponseSubmissionForFixedLevelOpenSetSilentIntervalsTest) {
    assertCompleteTrialShowsResponseView(
        confirmingFixedLevelOpenSetSilentIntervalsTest,
        respondingFromExperimenter);
}

PRESENTER_TEST(confirmAdaptiveClosedSetTestWithInvalidSnrShowsErrorMessage) {
    assertInvalidSnrShowsErrorMessage(confirmingDefaultAdaptiveClosedSetTest);
}

PRESENTER_TEST(confirmAdaptiveOpenSetTestWithInvalidSnrShowsErrorMessage) {
    assertInvalidSnrShowsErrorMessage(confirmingAdaptiveOpenSetTest);
}

PRESENTER_TEST(confirmFixedLevelOpenSetTestWithInvalidSnrShowsErrorMessage) {
    assertInvalidSnrShowsErrorMessage(confirmingDefaultFixedLevelOpenSetTest);
}

PRESENTER_TEST(confirmFixedLevelClosedSetTestWithInvalidSnrShowsErrorMessage) {
    assertInvalidSnrShowsErrorMessage(confirmingDefaultFixedLevelClosedSetTest);
}

PRESENTER_TEST(
    confirmAdaptiveClosedSetTestWithInvalidInputDoesNotHideSetupView) {
    assertSetupViewNotHiddenWhenSnrIsInvalid(
        confirmingDefaultAdaptiveClosedSetTest);
}

PRESENTER_TEST(confirmAdaptiveOpenSetTestWithInvalidInputDoesNotHideSetupView) {
    assertSetupViewNotHiddenWhenSnrIsInvalid(confirmingAdaptiveOpenSetTest);
}

PRESENTER_TEST(
    confirmFixedLevelOpenSetTestWithInvalidInputDoesNotHideSetupView) {
    assertSetupViewNotHiddenWhenSnrIsInvalid(
        confirmingDefaultFixedLevelOpenSetTest);
}

PRESENTER_TEST(
    confirmFixedLevelClosedSetTestWithInvalidInputDoesNotHideSetupView) {
    assertSetupViewNotHiddenWhenSnrIsInvalid(
        confirmingDefaultFixedLevelClosedSetTest);
}

PRESENTER_TEST(
    confirmFixedLevelClosedSetTestWithSilentIntervalTargetsWithInvalidInputDoesNotHideSetupView) {
    assertSetupViewNotHiddenWhenSnrIsInvalid(
        confirmingFixedLevelClosedSetSilentIntervalsTest);
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
