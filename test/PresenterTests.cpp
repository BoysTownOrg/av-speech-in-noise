#include "PresenterTests.hpp"

namespace av_speech_in_noise {
TEST_F(PresenterConstructionTests, populatesAudioDeviceMenu) {
    model.setAudioDevices({"a", "b", "c"});
    construct();
    assertEqual({"a", "b", "c"}, view.audioDevices());
}

#define PRESENTER_TEST(a) TEST_F(PresenterTests, a)

PRESENTER_TEST(confirmAdaptiveOpenSetKeywordsTestHidesTestSetupView) {
    assertHidesTestSetupView(confirmingAdaptiveCorrectKeywordsTest);
}

PRESENTER_TEST(confirmAdaptiveOpenSetKeywordsTestShowsExperimenterView) {
    assertShowsExperimenterView(confirmingAdaptiveCorrectKeywordsTest);
}

PRESENTER_TEST(confirmAdaptiveOpenSetKeywordsTestShowsTestingView) {
    assertShowsTestingView(confirmingAdaptiveCorrectKeywordsTest);
}

PRESENTER_TEST(confirmAdaptiveOpenSetKeywordsTestDoesNotShowSubjectView) {
    assertDoesNotShowSubjectView(confirmingAdaptiveCorrectKeywordsTest);
}

PRESENTER_TEST(
    confirmAdaptiveOpenSetKeywordsTestDoesNotInitializeFixedLevelTest) {
    assertDoesNotInitializeFixedLevelTest(
        confirmingAdaptiveCorrectKeywordsTest);
}

PRESENTER_TEST(confirmingAdaptiveCorrectKeywordsTestPassesStartingSnr) {
    assertStartingSnrPassedToModel(confirmingAdaptiveCorrectKeywordsTest);
}

PRESENTER_TEST(confirmingAdaptiveCorrectKeywordsTestPassesMaskerLevel) {
    assertMaskerLevelPassedToModel(confirmingAdaptiveCorrectKeywordsTest);
}

PRESENTER_TEST(
    confirmAdaptiveOpenSetKeywordsTestShowsNextTrialButtonForExperimenter) {
    assertConfirmTestSetupShowsNextTrialButton(
        confirmingAdaptiveCorrectKeywordsTest, playingTrialFromExperimenter);
}

PRESENTER_TEST(submittingCorrectKeywordsPassesCorrectKeywords) {
    setCorrectKeywords("1");
    run(submittingCorrectKeywords);
    assertEqual(1, model.correctKeywords());
}

PRESENTER_TEST(enteringInvalidCorrectKeywordsShowsErrorMessage) {
    setCorrectKeywords("a");
    run(submittingCorrectKeywords);
    assertErrorMessageEquals("'a' is not a valid number.");
}

PRESENTER_TEST(enteringInvalidCorrectKeywordsDoesNotHideEntry) {
    setCorrectKeywords("a");
    run(submittingCorrectKeywords);
    assertFalse(submittingCorrectKeywords.responseViewHidden());
}

PRESENTER_TEST(submittingCorrectKeywordsShowsSetupViewWhenTestComplete) {
    assertCompleteTestShowsSetupView(submittingCorrectKeywords);
}

PRESENTER_TEST(
    submittingCorrectKeywordsDoesNotShowSetupViewWhenTestIncomplete) {
    assertIncompleteTestDoesNotShowSetupView(submittingCorrectKeywords);
}

PRESENTER_TEST(submittingCorrectKeywordsHidesExperimenterViewWhenTestComplete) {
    assertCompleteTestHidesExperimenterView(submittingCorrectKeywords);
}

PRESENTER_TEST(submittingCorrectKeywordsHidesTestingViewWhenTestComplete) {
    assertCompleteTestHidesTestingView(submittingCorrectKeywords);
}

PRESENTER_TEST(
    submittingCorrectKeywordsDoesNotHideExperimenterViewWhenTestIncomplete) {
    assertDoesNotHideExperimenterView(submittingCorrectKeywords);
}

PRESENTER_TEST(
    submittingCorrectKeywordsDoesNotHideTestingViewWhenTestIncomplete) {
    assertDoesNotHideTestingView(submittingCorrectKeywords);
}

PRESENTER_TEST(submittingCorrectKeywordsShowsNextTrialButton) {
    assertShowsNextTrialButton(submittingCorrectKeywords);
}

PRESENTER_TEST(submittingCorrectKeywordsHidesCorrectKeywordsEntry) {
    assertResponseViewHidden(submittingCorrectKeywords);
}

PRESENTER_TEST(confirmAdaptiveOpenSetKeywordsTestShowsTrialNumber) {
    assertShowsTrialNumber(confirmingAdaptiveCorrectKeywordsTest);
}

PRESENTER_TEST(submittingCorrectKeywordsShowsTrialNumber) {
    assertShowsTrialNumber(submittingCorrectKeywords);
}

PRESENTER_TEST(
    completingTrialShowsExperimenterCorrectKeywordsEntryForAdaptiveOpenSetKeywordsTest) {
    assertCompleteTrialShowsResponseView(
        confirmingAdaptiveCorrectKeywordsTest, submittingCorrectKeywords);
}

PRESENTER_TEST(
    confirmAdaptiveOpenSetKeywordsTestWithInvalidSnrShowsErrorMessage) {
    assertInvalidSnrShowsErrorMessage(confirmingAdaptiveCorrectKeywordsTest);
}

PRESENTER_TEST(
    confirmAdaptiveOpenSetKeywordsTestWithInvalidInputDoesNotHideSetupView) {
    assertSetupViewNotHiddenWhenSnrIsInvalid(
        confirmingAdaptiveCorrectKeywordsTest);
}

PRESENTER_TEST(populatesConditionMenu) {
    assertSetupViewConditionsContains(auditoryOnlyConditionName());
    assertSetupViewConditionsContains(audioVisualConditionName());
}

PRESENTER_TEST(populatesMethodMenu) {
    assertSetupViewMethodsContains(Method::adaptivePassFail);
    assertSetupViewMethodsContains(Method::adaptiveCorrectKeywords);
    assertSetupViewMethodsContains(
        Method::defaultAdaptiveCoordinateResponseMeasure);
    assertSetupViewMethodsContains(
        Method::adaptiveCoordinateResponseMeasureWithSingleSpeaker);
    assertSetupViewMethodsContains(
        Method::adaptiveCoordinateResponseMeasureWithDelayedMasker);
    assertSetupViewMethodsContains(
        Method::adaptiveCoordinateResponseMeasureWithEyeTracking);
    assertSetupViewMethodsContains(
        Method::fixedLevelFreeResponseWithTargetReplacement);
    assertSetupViewMethodsContains(
        Method::fixedLevelFreeResponseWithAllTargets);
    assertSetupViewMethodsContains(
        Method::fixedLevelFreeResponseWithSilentIntervalTargets);
    assertSetupViewMethodsContains(
        Method::defaultFixedLevelCoordinateResponseMeasure);
    assertSetupViewMethodsContains(
        Method::fixedLevelCoordinateResponseMeasureWithSilentIntervalTargets);
}

PRESENTER_TEST(callsEventLoopWhenRun) {
    presenter.run();
    assertTrue(view.eventLoopCalled());
}

PRESENTER_TEST(
    confirmDefaultAdaptiveCoordinateResponseMeasureTestHidesTestSetupView) {
    assertHidesTestSetupView(
        confirmingDefaultAdaptiveCoordinateResponseMeasureTest);
}

PRESENTER_TEST(
    confirmAdaptiveCoordinateResponseMeasureTestWithSingleSpeakerHidesTestSetupView) {
    assertHidesTestSetupView(
        confirmingAdaptiveCoordinateResponseMeasureTestWithSingleSpeaker);
}

PRESENTER_TEST(
    confirmAdaptiveCoordinateResponseMeasureTestWithDelayedMaskerHidesTestSetupView) {
    assertHidesTestSetupView(
        confirmingAdaptiveCoordinateResponseMeasureTestWithDelayedMasker);
}

PRESENTER_TEST(
    confirmAdaptiveCoordinateResponseMeasureTestWithEyeTrackingHidesTestSetupView) {
    assertHidesTestSetupView(
        confirmingAdaptiveCoordinateResponseMeasureTestWithEyeTracking);
}

PRESENTER_TEST(confirmAdaptiveOpenSetTestHidesTestSetupView) {
    assertHidesTestSetupView(confirmingAdaptivePassFailTest);
}

PRESENTER_TEST(confirmFixedLevelOpenSetTestHidesTestSetupView) {
    assertHidesTestSetupView(
        confirmingFixedLevelFreeResponseWithTargetReplacementTest);
}

PRESENTER_TEST(
    confirmFixedLevelCoordinateResponseMeasureTestHidesTestSetupView) {
    assertHidesTestSetupView(
        confirmingDefaultFixedLevelCoordinateResponseMeasureTest);
}

PRESENTER_TEST(
    confirmFixedLevelCoordinateResponseMeasureTestWithSilentIntervalTargetsHidesTestSetupView) {
    assertHidesTestSetupView(
        confirmingFixedLevelCoordinateResponseMeasureSilentIntervalsTest);
}

PRESENTER_TEST(confirmAdaptiveOpenSetTestShowsExperimenterView) {
    assertShowsExperimenterView(confirmingAdaptivePassFailTest);
}

PRESENTER_TEST(confirmAdaptiveOpenSetTestShowsTestingView) {
    assertShowsTestingView(confirmingAdaptivePassFailTest);
}

PRESENTER_TEST(confirmFixedLevelOpenSetTestShowsTestingView) {
    assertShowsTestingView(
        confirmingFixedLevelFreeResponseWithTargetReplacementTest);
}

PRESENTER_TEST(confirmFixedLevelOpenSetTestShowsExperimenterView) {
    assertShowsExperimenterView(
        confirmingFixedLevelFreeResponseWithTargetReplacementTest);
}

PRESENTER_TEST(
    confirmDefaultAdaptiveCoordinateResponseMeasureTestShowsSubjectView) {
    assertShowsSubjectView(
        confirmingDefaultAdaptiveCoordinateResponseMeasureTest);
}

PRESENTER_TEST(
    confirmAdaptiveCoordinateResponseMeasureTestWithSingleSpeakerShowsSubjectView) {
    assertShowsSubjectView(
        confirmingAdaptiveCoordinateResponseMeasureTestWithSingleSpeaker);
}

PRESENTER_TEST(
    confirmAdaptiveCoordinateResponseMeasureTestWithDelayedMaskerShowsSubjectView) {
    assertShowsSubjectView(
        confirmingAdaptiveCoordinateResponseMeasureTestWithDelayedMasker);
}

PRESENTER_TEST(
    confirmAdaptiveCoordinateResponseMeasureTestWithEyeTrackingShowsSubjectView) {
    assertShowsSubjectView(
        confirmingAdaptiveCoordinateResponseMeasureTestWithEyeTracking);
}

PRESENTER_TEST(
    confirmDefaultAdaptiveCoordinateResponseMeasureTestDoesNotShowSubjectViewWhenTestComplete) {
    setTestComplete();
    assertDoesNotShowSubjectView(
        confirmingDefaultAdaptiveCoordinateResponseMeasureTest);
}

PRESENTER_TEST(
    confirmDefaultAdaptiveCoordinateResponseMeasureTestDoesNotHideSetupViewWhenTestComplete) {
    setTestComplete();
    assertDoesNotHideTestSetupView(
        confirmingDefaultAdaptiveCoordinateResponseMeasureTest);
}

PRESENTER_TEST(confirmFixedLevelCoordinateResponseMeasureTestShowsSubjectView) {
    assertShowsSubjectView(
        confirmingDefaultFixedLevelCoordinateResponseMeasureTest);
}

PRESENTER_TEST(
    confirmFixedLevelCoordinateResponseMeasureTestWithSilentIntervalTargetsShowsSubjectView) {
    assertShowsSubjectView(
        confirmingFixedLevelCoordinateResponseMeasureSilentIntervalsTest);
}

PRESENTER_TEST(confirmAdaptiveOpenSetTestDoesNotShowSubjectView) {
    assertDoesNotShowSubjectView(confirmingAdaptivePassFailTest);
}

PRESENTER_TEST(confirmFixedLevelOpenSetTestDoesNotShowSubjectView) {
    assertDoesNotShowSubjectView(
        confirmingFixedLevelFreeResponseWithTargetReplacementTest);
}

PRESENTER_TEST(
    confirmFixedLevelCoordinateResponseMeasureTestWithAllTargetsDoesNotShowSubjectView) {
    assertDoesNotShowSubjectView(
        confirmingFixedLevelFreeResponseTestWithAllTargets);
}

PRESENTER_TEST(
    confirmFixedLevelOpenSetTestWithSilentIntervalTargetsDoesNotShowSubjectView) {
    assertDoesNotShowSubjectView(
        confirmingFixedLevelFreeResponseWithSilentIntervalTargetsTest);
}

PRESENTER_TEST(
    confirmDefaultAdaptiveCoordinateResponseMeasureTestShowsExperimenterView) {
    assertShowsExperimenterView(
        confirmingDefaultAdaptiveCoordinateResponseMeasureTest);
}

PRESENTER_TEST(
    confirmFixedLevelCoordinateResponseMeasureTestShowsExperimenterView) {
    assertShowsExperimenterView(
        confirmingDefaultFixedLevelCoordinateResponseMeasureTest);
}

PRESENTER_TEST(
    confirmFixedLevelCoordinateResponseMeasureTestWithSilentIntervalTargetsShowsExperimenterView) {
    assertShowsExperimenterView(
        confirmingFixedLevelCoordinateResponseMeasureSilentIntervalsTest);
}

PRESENTER_TEST(
    confirmAdaptiveCoordinateResponseMeasureTestWithSingleSpeakerInitializesModel) {
    run(confirmingAdaptiveCoordinateResponseMeasureTestWithSingleSpeaker);
    assertTrue(model.initializedWithSingleSpeaker());
}

PRESENTER_TEST(
    confirmAdaptiveCoordinateResponseMeasureTestWithDelayedMaskerInitializesModel) {
    run(confirmingAdaptiveCoordinateResponseMeasureTestWithDelayedMasker);
    assertTrue(model.initializedWithDelayedMasker());
}

PRESENTER_TEST(
    confirmAdaptiveCoordinateResponseMeasureTestWithEyeTrackingInitializesModel) {
    run(confirmingAdaptiveCoordinateResponseMeasureTestWithEyeTracking);
    assertTrue(model.initializedWithEyeTracking());
}

PRESENTER_TEST(
    confirmFixedLevelCoordinateResponseMeasureTestWithSilentIntervalTargetsInitializesModel) {
    run(confirmingFixedLevelCoordinateResponseMeasureSilentIntervalsTest);
    assertTrue(model.fixedLevelTestWithSilentIntervalTargetsInitialized());
}

PRESENTER_TEST(
    confirmFixedLevelCoordinateResponseMeasureTestWithAllTargetsInitializesModel) {
    run(confirmingFixedLevelFreeResponseTestWithAllTargets);
    assertTrue(model.fixedLevelTestWithAllTargetsInitialized());
}

PRESENTER_TEST(
    confirmFixedLevelOpenSetTestWithSilentIntervalTargetsInitializesModel) {
    run(confirmingFixedLevelFreeResponseWithSilentIntervalTargetsTest);
    assertTrue(model.fixedLevelTestWithSilentIntervalTargetsInitialized());
}

PRESENTER_TEST(
    confirmDefaultAdaptiveCoordinateResponseMeasureTestDoesNotInitializeFixedLevelTest) {
    assertDoesNotInitializeFixedLevelTest(
        confirmingDefaultAdaptiveCoordinateResponseMeasureTest);
}

PRESENTER_TEST(confirmAdaptiveOpenSetTestDoesNotInitializeFixedLevelTest) {
    assertDoesNotInitializeFixedLevelTest(confirmingAdaptivePassFailTest);
}

PRESENTER_TEST(confirmFixedLevelOpenSetTestDoesNotInitializeAdaptiveTest) {
    assertDoesNotInitializeAdaptiveTest(
        confirmingFixedLevelFreeResponseWithTargetReplacementTest);
}

PRESENTER_TEST(
    confirmFixedLevelCoordinateResponseMeasureTestWithAllTargetsDoesNotInitializeAdaptiveTest) {
    assertDoesNotInitializeAdaptiveTest(
        confirmingFixedLevelFreeResponseTestWithAllTargets);
}

PRESENTER_TEST(
    confirmFixedLevelOpenSetTestWithSilentIntervalTargetsDoesNotInitializeAdaptiveTest) {
    assertDoesNotInitializeAdaptiveTest(
        confirmingFixedLevelFreeResponseWithSilentIntervalTargetsTest);
}

PRESENTER_TEST(
    confirmFixedLevelCoordinateResponseMeasureTestDoesNotInitializeAdaptiveTest) {
    assertDoesNotInitializeAdaptiveTest(
        confirmingDefaultFixedLevelCoordinateResponseMeasureTest);
}

PRESENTER_TEST(
    confirmFixedLevelCoordinateResponseMeasureTestWithSilentIntervalTargetsDoesNotInitializeAdaptiveTest) {
    assertDoesNotInitializeAdaptiveTest(
        confirmingFixedLevelCoordinateResponseMeasureSilentIntervalsTest);
}

PRESENTER_TEST(confirmAdaptiveCoordinateResponseMeasureTestPassesStartingSnr) {
    assertStartingSnrPassedToModel(
        confirmingDefaultAdaptiveCoordinateResponseMeasureTest);
}

PRESENTER_TEST(confirmAdaptiveOpenSetTestPassesStartingSnr) {
    assertStartingSnrPassedToModel(confirmingAdaptivePassFailTest);
}

PRESENTER_TEST(confirmFixedLevelOpenSetTestPassesStartingSnr) {
    assertStartingSnrPassedToModel(
        confirmingFixedLevelFreeResponseWithTargetReplacementTest);
}

PRESENTER_TEST(
    confirmFixedLevelCoordinateResponseMeasureTestPassesStartingSnr) {
    assertStartingSnrPassedToModel(
        confirmingDefaultFixedLevelCoordinateResponseMeasureTest);
}

PRESENTER_TEST(
    confirmFixedLevelCoordinateResponseMeasureTestWithSilentIntervalTargetsPassesStartingSnr) {
    assertStartingSnrPassedToModel(
        confirmingFixedLevelCoordinateResponseMeasureSilentIntervalsTest);
}

PRESENTER_TEST(confirmAdaptiveCoordinateResponseMeasureTestPassesMaskerLevel) {
    assertMaskerLevelPassedToModel(
        confirmingDefaultAdaptiveCoordinateResponseMeasureTest);
}

PRESENTER_TEST(confirmAdaptiveOpenSetTestPassesMaskerLevel) {
    assertMaskerLevelPassedToModel(confirmingAdaptivePassFailTest);
}

PRESENTER_TEST(confirmFixedLevelOpenSetTestPassesMaskerLevel) {
    assertMaskerLevelPassedToModel(
        confirmingFixedLevelFreeResponseWithTargetReplacementTest);
}

PRESENTER_TEST(
    confirmFixedLevelCoordinateResponseMeasureTestPassesMaskerLevel) {
    assertMaskerLevelPassedToModel(
        confirmingDefaultFixedLevelCoordinateResponseMeasureTest);
}

PRESENTER_TEST(playCalibrationPassesLevel) {
    setCalibrationLevel("1");
    playCalibration();
    assertEqual(1, calibration().level_dB_SPL);
}

PRESENTER_TEST(confirmAdaptiveCoordinateResponseMeasureTestPassesTargetList) {
    assertPassesTargetListDirectory(
        confirmingDefaultAdaptiveCoordinateResponseMeasureTest);
}

PRESENTER_TEST(confirmAdaptiveOpenSetTestPassesTargetList) {
    assertPassesTargetListDirectory(confirmingAdaptivePassFailTest);
}

PRESENTER_TEST(confirmFixedLevelOpenSetTestPassesTargetList) {
    assertPassesTargetListDirectory(
        confirmingFixedLevelFreeResponseWithTargetReplacementTest);
}

PRESENTER_TEST(confirmFixedLevelCoordinateResponseMeasureTestPassesTargetList) {
    assertPassesTargetListDirectory(
        confirmingDefaultFixedLevelCoordinateResponseMeasureTest);
}

PRESENTER_TEST(confirmAdaptiveCoordinateResponseMeasureTestPassesSubjectId) {
    assertPassesSubjectId(
        confirmingDefaultAdaptiveCoordinateResponseMeasureTest);
}

PRESENTER_TEST(confirmAdaptiveOpenSetTestPassesSubjectId) {
    assertPassesSubjectId(confirmingAdaptivePassFailTest);
}

PRESENTER_TEST(confirmFixedLevelOpenSetTestPassesSubjectId) {
    assertPassesSubjectId(
        confirmingFixedLevelFreeResponseWithTargetReplacementTest);
}

PRESENTER_TEST(confirmFixedLevelCoordinateResponseMeasureTestPassesSubjectId) {
    assertPassesSubjectId(
        confirmingDefaultFixedLevelCoordinateResponseMeasureTest);
}

PRESENTER_TEST(confirmAdaptiveCoordinateResponseMeasureTestPassesTesterId) {
    assertPassesTesterId(
        confirmingDefaultAdaptiveCoordinateResponseMeasureTest);
}

PRESENTER_TEST(confirmAdaptiveOpenSetTestPassesTesterId) {
    assertPassesTesterId(confirmingAdaptivePassFailTest);
}

PRESENTER_TEST(confirmFixedLevelOpenSetTestPassesTesterId) {
    assertPassesTesterId(
        confirmingFixedLevelFreeResponseWithTargetReplacementTest);
}

PRESENTER_TEST(confirmFixedLevelCoordinateResponseMeasureTestPassesTesterId) {
    assertPassesTesterId(
        confirmingDefaultFixedLevelCoordinateResponseMeasureTest);
}

PRESENTER_TEST(confirmAdaptiveCoordinateResponseMeasureTestPassesMasker) {
    assertPassesMasker(confirmingDefaultAdaptiveCoordinateResponseMeasureTest);
}

PRESENTER_TEST(confirmAdaptiveOpenSetTestPassesMasker) {
    assertPassesMasker(confirmingAdaptivePassFailTest);
}

PRESENTER_TEST(confirmFixedLevelOpenSetTestPassesMasker) {
    assertPassesMasker(
        confirmingFixedLevelFreeResponseWithTargetReplacementTest);
}

PRESENTER_TEST(confirmFixedLevelCoordinateResponseMeasureTestPassesMasker) {
    assertPassesMasker(
        confirmingDefaultFixedLevelCoordinateResponseMeasureTest);
}

PRESENTER_TEST(playCalibrationPassesFilePath) {
    setupView.setCalibrationFilePath("a");
    playCalibration();
    assertEqual("a", calibration().filePath);
}

PRESENTER_TEST(confirmAdaptiveCoordinateResponseMeasureTestPassesSession) {
    assertPassesSession(confirmingDefaultAdaptiveCoordinateResponseMeasureTest);
}

PRESENTER_TEST(confirmAdaptiveOpenSetTestPassesSession) {
    assertPassesSession(confirmingAdaptivePassFailTest);
}

PRESENTER_TEST(confirmFixedLevelOpenSetTestPassesSession) {
    assertPassesSession(
        confirmingFixedLevelFreeResponseWithTargetReplacementTest);
}

PRESENTER_TEST(confirmFixedLevelCoordinateResponseMeasureTestPassesSession) {
    assertPassesSession(
        confirmingDefaultFixedLevelCoordinateResponseMeasureTest);
}

PRESENTER_TEST(confirmAdaptiveCoordinateResponseMeasureTestPassesMethod) {
    assertPassesMethod(confirmingDefaultAdaptiveCoordinateResponseMeasureTest);
}

PRESENTER_TEST(confirmAdaptiveOpenSetTestPassesMethod) {
    assertPassesMethod(confirmingAdaptivePassFailTest);
}

PRESENTER_TEST(confirmFixedLevelOpenSetTestPassesMethod) {
    assertPassesMethod(
        confirmingFixedLevelFreeResponseWithTargetReplacementTest);
}

PRESENTER_TEST(confirmFixedLevelCoordinateResponseMeasureTestPassesMethod) {
    assertPassesMethod(
        confirmingDefaultFixedLevelCoordinateResponseMeasureTest);
}

PRESENTER_TEST(
    confirmAdaptiveCoordinateResponseMeasureTestWithDelayedMaskerPassesMethod) {
    assertPassesMethod(
        confirmingAdaptiveCoordinateResponseMeasureTestWithDelayedMasker);
}

PRESENTER_TEST(
    confirmAdaptiveCoordinateResponseMeasureTestWithEyeTrackingPassesMethod) {
    assertPassesMethod(
        confirmingAdaptiveCoordinateResponseMeasureTestWithEyeTracking);
}

PRESENTER_TEST(
    confirmAdaptiveCoordinateResponseMeasureSingleSpeakerTestPassesMethod) {
    assertPassesMethod(
        confirmingAdaptiveCoordinateResponseMeasureTestWithSingleSpeaker);
}

PRESENTER_TEST(confirmAdaptiveCoordinateResponseMeasureTestPassesCeilingSNR) {
    assertPassesCeilingSNR(
        confirmingDefaultAdaptiveCoordinateResponseMeasureTest);
}

PRESENTER_TEST(confirmAdaptiveOpenSetTestPassesCeilingSNR) {
    assertPassesCeilingSNR(confirmingAdaptivePassFailTest);
}

PRESENTER_TEST(confirmAdaptiveCoordinateResponseMeasureTestPassesFloorSNR) {
    assertPassesFloorSNR(
        confirmingDefaultAdaptiveCoordinateResponseMeasureTest);
}

PRESENTER_TEST(confirmAdaptiveOpenSetTestPassesFloorSNR) {
    assertPassesFloorSNR(confirmingAdaptivePassFailTest);
}

PRESENTER_TEST(
    confirmAdaptiveCoordinateResponseMeasureTestPassesTrackBumpLimit) {
    assertPassesTrackBumpLimit(
        confirmingDefaultAdaptiveCoordinateResponseMeasureTest);
}

PRESENTER_TEST(confirmAdaptiveOpenSetTestPassesTrackBumpLimit) {
    assertPassesTrackBumpLimit(confirmingAdaptivePassFailTest);
}

PRESENTER_TEST(
    confirmAdaptiveCoordinateResponseMeasureTestPassesFullScaleLevel) {
    assertPassesFullScaleLevel(
        confirmingDefaultAdaptiveCoordinateResponseMeasureTest);
}

PRESENTER_TEST(confirmAdaptiveOpenSetTestPassesFullScaleLevel) {
    assertPassesFullScaleLevel(confirmingAdaptivePassFailTest);
}

PRESENTER_TEST(confirmFixedLevelOpenSetTestPassesFullScaleLevel) {
    assertPassesFullScaleLevel(
        confirmingFixedLevelFreeResponseWithTargetReplacementTest);
}

PRESENTER_TEST(
    confirmFixedLevelCoordinateResponseMeasureTestPassesFullScaleLevel) {
    assertPassesFullScaleLevel(
        confirmingDefaultFixedLevelCoordinateResponseMeasureTest);
}

PRESENTER_TEST(playCalibrationPassesFullScaleLevel) {
    assertPassesFullScaleLevel(playingCalibration);
}

PRESENTER_TEST(
    confirmAdaptiveCoordinateResponseMeasureTestPassesTrackSettingsFile) {
    assertPassesTrackSettingsFile(
        confirmingDefaultAdaptiveCoordinateResponseMeasureTest);
}

PRESENTER_TEST(confirmAdaptiveOpenSetTestPassesTrackSettingsFile) {
    assertPassesTrackSettingsFile(confirmingAdaptivePassFailTest);
}

PRESENTER_TEST(
    confirmAdaptiveCoordinateResponseMeasureTestPassesAudioVisualCondition) {
    assertAudioVisualConditionPassedToModel(
        confirmingDefaultAdaptiveCoordinateResponseMeasureTest);
}

PRESENTER_TEST(
    confirmAdaptiveCoordinateResponseMeasureTestPassesAuditoryOnlyCondition) {
    assertAuditoryOnlyConditionPassedToModel(
        confirmingDefaultAdaptiveCoordinateResponseMeasureTest);
}

PRESENTER_TEST(confirmAdaptiveOpenSetTestPassesAudioVisualCondition) {
    assertAudioVisualConditionPassedToModel(confirmingAdaptivePassFailTest);
}

PRESENTER_TEST(confirmAdaptiveOpenSetTestPassesAuditoryOnlyCondition) {
    assertAuditoryOnlyConditionPassedToModel(confirmingAdaptivePassFailTest);
}

PRESENTER_TEST(confirmFixedLevelOpenSetTestPassesAudioVisualCondition) {
    assertAudioVisualConditionPassedToModel(
        confirmingFixedLevelFreeResponseWithTargetReplacementTest);
}

PRESENTER_TEST(confirmFixedLevelOpenSetTestPassesAuditoryOnlyCondition) {
    assertAuditoryOnlyConditionPassedToModel(
        confirmingFixedLevelFreeResponseWithTargetReplacementTest);
}

PRESENTER_TEST(
    confirmFixedLevelCoordinateResponseMeasureTestPassesAudioVisualCondition) {
    assertAudioVisualConditionPassedToModel(
        confirmingDefaultFixedLevelCoordinateResponseMeasureTest);
}

PRESENTER_TEST(confirmFixedLevelClosedetTestPassesAuditoryOnlyCondition) {
    assertAuditoryOnlyConditionPassedToModel(
        confirmingDefaultFixedLevelCoordinateResponseMeasureTest);
}

PRESENTER_TEST(playCalibrationPassesAudioVisualCondition) {
    assertAudioVisualConditionPassedToModel(playingCalibration);
}

PRESENTER_TEST(playCalibrationPassesAuditoryOnlyCondition) {
    assertAuditoryOnlyConditionPassedToModel(playingCalibration);
}

PRESENTER_TEST(
    confirmDefaultAdaptiveCoordinateResponseMeasureTestShowsNextTrialButtonForSubject) {
    assertConfirmTestSetupShowsNextTrialButton(
        confirmingDefaultAdaptiveCoordinateResponseMeasureTest,
        playingTrialFromSubject);
}

PRESENTER_TEST(
    confirmFixedLevelCoordinateResponseMeasureTestShowsNextTrialButtonForSubject) {
    assertConfirmTestSetupShowsNextTrialButton(
        confirmingDefaultFixedLevelCoordinateResponseMeasureTest,
        playingTrialFromSubject);
}

PRESENTER_TEST(
    confirmFixedLevelCoordinateResponseMeasureTestWithSilentIntervalTargetsShowsNextTrialButtonForSubject) {
    assertConfirmTestSetupShowsNextTrialButton(
        confirmingFixedLevelCoordinateResponseMeasureSilentIntervalsTest,
        playingTrialFromSubject);
}

PRESENTER_TEST(confirmAdaptiveOpenSetTestShowsNextTrialButtonForExperimenter) {
    assertConfirmTestSetupShowsNextTrialButton(
        confirmingAdaptivePassFailTest, playingTrialFromExperimenter);
}

PRESENTER_TEST(
    confirmFixedLevelOpenSetTestShowsNextTrialButtonForExperimenter) {
    assertConfirmTestSetupShowsNextTrialButton(
        confirmingFixedLevelFreeResponseWithTargetReplacementTest,
        playingTrialFromExperimenter);
}

PRESENTER_TEST(
    confirmFixedLevelCoordinateResponseMeasureTestWithAllTargetsShowsNextTrialButtonForExperimenter) {
    assertConfirmTestSetupShowsNextTrialButton(
        confirmingFixedLevelFreeResponseTestWithAllTargets,
        playingTrialFromExperimenter);
}

PRESENTER_TEST(
    confirmFixedLevelOpenSetTestWithSilentIntervalTargetsShowsNextTrialButtonForExperimenter) {
    assertConfirmTestSetupShowsNextTrialButton(
        confirmingFixedLevelFreeResponseWithSilentIntervalTargetsTest,
        playingTrialFromExperimenter);
}

PRESENTER_TEST(
    confirmingAdaptiveCoordinateResponseMeasureTestWithInvalidMaskerLevelShowsErrorMessage) {
    assertInvalidMaskerLevelShowsErrorMessage(
        confirmingDefaultAdaptiveCoordinateResponseMeasureTest);
}

PRESENTER_TEST(
    confirmingAdaptivePassFailTestWithInvalidMaskerLevelShowsErrorMessage) {
    assertInvalidMaskerLevelShowsErrorMessage(confirmingAdaptivePassFailTest);
}

PRESENTER_TEST(
    confirmingFixedLevelOpenSetTestWithInvalidMaskerLevelShowsErrorMessage) {
    assertInvalidMaskerLevelShowsErrorMessage(
        confirmingFixedLevelFreeResponseWithTargetReplacementTest);
}

PRESENTER_TEST(
    confirmingFixedLevelCoordinateResponseMeasureTestWithInvalidMaskerLevelShowsErrorMessage) {
    assertInvalidMaskerLevelShowsErrorMessage(
        confirmingDefaultFixedLevelCoordinateResponseMeasureTest);
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
    assertCompleteTestShowsSetupView(submittingFreeResponse);
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
    assertIncompleteTestDoesNotShowSetupView(submittingFreeResponse);
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
    assertCompleteTestHidesExperimenterView(submittingFreeResponse);
}

PRESENTER_TEST(submitPassedTrialHidesExperimenterViewWhenTestComplete) {
    assertCompleteTestHidesExperimenterView(submittingPassedTrial);
}

PRESENTER_TEST(submitFailedTrialHidesExperimenterViewWhenTestComplete) {
    assertCompleteTestHidesExperimenterView(submittingFailedTrial);
}

PRESENTER_TEST(respondFromExperimenterHidesTestingViewWhenTestComplete) {
    assertCompleteTestHidesTestingView(submittingFreeResponse);
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
    assertDoesNotHideExperimenterView(submittingFreeResponse);
}

PRESENTER_TEST(submitPassedTrialDoesNotHideExperimenterViewWhenTestIncomplete) {
    assertDoesNotHideExperimenterView(submittingPassedTrial);
}

PRESENTER_TEST(submitFailedTrialDoesNotHideExperimenterViewWhenTestIncomplete) {
    assertDoesNotHideExperimenterView(submittingFailedTrial);
}

PRESENTER_TEST(
    respondFromExperimenterDoesNotHideTestingViewWhenTestIncomplete) {
    assertDoesNotHideTestingView(submittingFreeResponse);
}

PRESENTER_TEST(submitPassedTrialDoesNotHideTestingViewWhenTestIncomplete) {
    assertDoesNotHideTestingView(submittingPassedTrial);
}

PRESENTER_TEST(submitFailedTrialDoesNotHideTestingViewWhenTestIncomplete) {
    assertDoesNotHideTestingView(submittingFailedTrial);
}

PRESENTER_TEST(experimenterResponseShowsNextTrialButton) {
    assertShowsNextTrialButton(submittingFreeResponse);
}

PRESENTER_TEST(subjectPassedTrialShowsNextTrialButtonForExperimenter) {
    assertShowsNextTrialButton(submittingPassedTrial);
}

PRESENTER_TEST(subjectFailedTrialShowsNextTrialButtonForExperimenter) {
    assertShowsNextTrialButton(submittingFailedTrial);
}

PRESENTER_TEST(experimenterResponseHidesResponseSubmission) {
    assertResponseViewHidden(submittingFreeResponse);
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
    assertShowsTrialNumber(
        confirmingFixedLevelFreeResponseWithTargetReplacementTest);
}

PRESENTER_TEST(
    confirmFixedLevelCoordinateResponseMeasureTestWithAllTargetsShowsTrialNumber) {
    assertShowsTrialNumber(confirmingFixedLevelFreeResponseTestWithAllTargets);
}

PRESENTER_TEST(
    confirmFixedLevelOpenSetTestWithSilentIntervalTargetsShowsTrialNumber) {
    assertShowsTrialNumber(
        confirmingFixedLevelFreeResponseWithSilentIntervalTargetsTest);
}

PRESENTER_TEST(confirmAdaptiveOpenSetTestShowsTrialNumber) {
    assertShowsTrialNumber(confirmingAdaptivePassFailTest);
}

PRESENTER_TEST(confirmFixedLevelCoordinateResponseMeasureTestShowsTrialNumber) {
    assertShowsTrialNumber(
        confirmingDefaultFixedLevelCoordinateResponseMeasureTest);
}

PRESENTER_TEST(
    confirmFixedLevelCoordinateResponseMeasureTestWithSilentIntervalTargetsShowsTrialNumber) {
    assertShowsTrialNumber(
        confirmingFixedLevelCoordinateResponseMeasureSilentIntervalsTest);
}

PRESENTER_TEST(
    confirmDefaultAdaptiveCoordinateResponseMeasureTestShowsTrialNumber) {
    assertShowsTrialNumber(
        confirmingDefaultAdaptiveCoordinateResponseMeasureTest);
}

PRESENTER_TEST(submittingResponseFromExperimenterShowsTrialNumber) {
    assertShowsTrialNumber(submittingFreeResponse);
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
    completingTrialShowsSubjectResponseButtonsForAdaptiveCoordinateResponseMeasureTest) {
    assertCompleteTrialShowsResponseView(
        confirmingDefaultAdaptiveCoordinateResponseMeasureTest,
        respondingFromSubject);
}

PRESENTER_TEST(
    completingTrialShowsSubjectResponseButtonsForAdaptiveCoordinateResponseMeasureSingleSpeakerTest) {
    assertCompleteTrialShowsResponseView(
        confirmingAdaptiveCoordinateResponseMeasureTestWithSingleSpeaker,
        respondingFromSubject);
}

PRESENTER_TEST(
    completingTrialShowsSubjectResponseButtonsForAdaptiveCoordinateResponseMeasureTestWithDelayedMasker) {
    assertCompleteTrialShowsResponseView(
        confirmingAdaptiveCoordinateResponseMeasureTestWithDelayedMasker,
        respondingFromSubject);
}

PRESENTER_TEST(
    completingTrialShowsSubjectResponseButtonsForAdaptiveCoordinateResponseMeasureTestWithEyeTracking) {
    assertCompleteTrialShowsResponseView(
        confirmingAdaptiveCoordinateResponseMeasureTestWithEyeTracking,
        respondingFromSubject);
}

PRESENTER_TEST(
    completingTrialShowsSubjectResponseButtonsForFixedLevelCoordinateResponseMeasureTest) {
    assertCompleteTrialShowsResponseView(
        confirmingDefaultFixedLevelCoordinateResponseMeasureTest,
        respondingFromSubject);
}

PRESENTER_TEST(
    completingTrialShowsSubjectResponseButtonsForFixedLevelCoordinateResponseMeasureSilentIntervalsTest) {
    assertCompleteTrialShowsResponseView(
        confirmingFixedLevelCoordinateResponseMeasureSilentIntervalsTest,
        respondingFromSubject);
}

PRESENTER_TEST(
    completingTrialShowsExperimenterEvaluationButtonsForAdaptiveOpenSetTest) {
    assertCompleteTrialShowsResponseView(
        confirmingAdaptivePassFailTest, submittingPassedTrial);
}

PRESENTER_TEST(
    completingTrialShowsExperimenterResponseSubmissionForFixedLevelOpenSetTest) {
    assertCompleteTrialShowsResponseView(
        confirmingFixedLevelFreeResponseWithTargetReplacementTest,
        submittingFreeResponse);
}

PRESENTER_TEST(
    completingTrialShowsExperimenterResponseSubmissionForFixedLevelOpenSetAllStimuliTest) {
    assertCompleteTrialShowsResponseView(
        confirmingFixedLevelFreeResponseTestWithAllTargets,
        submittingFreeResponse);
}

PRESENTER_TEST(
    completingTrialShowsExperimenterResponseSubmissionForFixedLevelOpenSetSilentIntervalsTest) {
    assertCompleteTrialShowsResponseView(
        confirmingFixedLevelFreeResponseWithSilentIntervalTargetsTest,
        submittingFreeResponse);
}

PRESENTER_TEST(
    confirmAdaptiveCoordinateResponseMeasureTestWithInvalidSnrShowsErrorMessage) {
    assertInvalidSnrShowsErrorMessage(
        confirmingDefaultAdaptiveCoordinateResponseMeasureTest);
}

PRESENTER_TEST(confirmAdaptiveOpenSetTestWithInvalidSnrShowsErrorMessage) {
    assertInvalidSnrShowsErrorMessage(confirmingAdaptivePassFailTest);
}

PRESENTER_TEST(confirmFixedLevelOpenSetTestWithInvalidSnrShowsErrorMessage) {
    assertInvalidSnrShowsErrorMessage(
        confirmingFixedLevelFreeResponseWithTargetReplacementTest);
}

PRESENTER_TEST(
    confirmFixedLevelCoordinateResponseMeasureTestWithInvalidSnrShowsErrorMessage) {
    assertInvalidSnrShowsErrorMessage(
        confirmingDefaultFixedLevelCoordinateResponseMeasureTest);
}

PRESENTER_TEST(
    confirmAdaptiveCoordinateResponseMeasureTestWithInvalidInputDoesNotHideSetupView) {
    assertSetupViewNotHiddenWhenSnrIsInvalid(
        confirmingDefaultAdaptiveCoordinateResponseMeasureTest);
}

PRESENTER_TEST(confirmAdaptiveOpenSetTestWithInvalidInputDoesNotHideSetupView) {
    assertSetupViewNotHiddenWhenSnrIsInvalid(confirmingAdaptivePassFailTest);
}

PRESENTER_TEST(
    confirmFixedLevelOpenSetTestWithInvalidInputDoesNotHideSetupView) {
    assertSetupViewNotHiddenWhenSnrIsInvalid(
        confirmingFixedLevelFreeResponseWithTargetReplacementTest);
}

PRESENTER_TEST(
    confirmFixedLevelCoordinateResponseMeasureTestWithInvalidInputDoesNotHideSetupView) {
    assertSetupViewNotHiddenWhenSnrIsInvalid(
        confirmingDefaultFixedLevelCoordinateResponseMeasureTest);
}

PRESENTER_TEST(
    confirmFixedLevelCoordinateResponseMeasureTestWithSilentIntervalTargetsWithInvalidInputDoesNotHideSetupView) {
    assertSetupViewNotHiddenWhenSnrIsInvalid(
        confirmingFixedLevelCoordinateResponseMeasureSilentIntervalsTest);
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
