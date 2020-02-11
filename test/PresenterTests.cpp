#include "PresenterTests.hpp"

namespace av_speech_in_noise {
TEST_F(PresenterConstructionTests, populatesAudioDeviceMenu) {
    model.setAudioDevices({"a", "b", "c"});
    construct();
    assertEqual({"a", "b", "c"}, view.audioDevices());
}

#define PRESENTER_TEST(a) TEST_F(PresenterTests, a)

PRESENTER_TEST(confirmingAdaptiveCorrectKeywordsTestHidesTestSetupView) {
    assertHidesTestSetupView(confirmingAdaptiveCorrectKeywordsTest);
}

PRESENTER_TEST(confirmingAdaptiveCorrectKeywordsTestShowsExperimenterView) {
    assertShowsExperimenterView(confirmingAdaptiveCorrectKeywordsTest);
}

PRESENTER_TEST(confirmingAdaptiveCorrectKeywordsTestShowsTestingView) {
    assertShowsTestingView(confirmingAdaptiveCorrectKeywordsTest);
}

PRESENTER_TEST(confirmingAdaptiveCorrectKeywordsTestDoesNotShowSubjectView) {
    assertDoesNotShowSubjectView(confirmingAdaptiveCorrectKeywordsTest);
}

PRESENTER_TEST(
    confirmingAdaptiveCorrectKeywordsTestDoesNotInitializeFixedLevelTest) {
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
    confirmingAdaptiveCorrectKeywordsTestShowsNextTrialButtonForExperimenter) {
    assertConfirmTestSetupShowsNextTrialButton(
        confirmingAdaptiveCorrectKeywordsTest, playingTrialFromExperimenter);
}

PRESENTER_TEST(submittingCorrectKeywordsPassesCorrectKeywords) {
    setCorrectKeywords("1");
    run(submittingCorrectKeywords);
    assertEqual(1, model.correctKeywords());
}

PRESENTER_TEST(submittingInvalidCorrectKeywordsShowsErrorMessage) {
    setCorrectKeywords("a");
    run(submittingCorrectKeywords);
    assertErrorMessageEquals("'a' is not a valid number.");
}

PRESENTER_TEST(submittingInvalidCorrectKeywordsDoesNotHideEntry) {
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

PRESENTER_TEST(confirmingAdaptiveCorrectKeywordsTestShowsTrialNumber) {
    assertShowsTrialNumber(confirmingAdaptiveCorrectKeywordsTest);
}

PRESENTER_TEST(submittingCorrectKeywordsShowsTrialNumber) {
    assertShowsTrialNumber(submittingCorrectKeywords);
}

PRESENTER_TEST(
    completingTrialShowsExperimenterCorrectKeywordsEntryForAdaptiveCorrectKeywordsTest) {
    assertCompleteTrialShowsResponseView(
        confirmingAdaptiveCorrectKeywordsTest, submittingCorrectKeywords);
}

PRESENTER_TEST(
    confirmingAdaptiveCorrectKeywordsTestWithInvalidSnrShowsErrorMessage) {
    assertInvalidSnrShowsErrorMessage(confirmingAdaptiveCorrectKeywordsTest);
}

PRESENTER_TEST(
    confirmingAdaptiveCorrectKeywordsTestWithInvalidInputDoesNotHideSetupView) {
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
    confirmingDefaultAdaptiveCoordinateResponseMeasureTestHidesTestSetupView) {
    assertHidesTestSetupView(
        confirmingDefaultAdaptiveCoordinateResponseMeasureTest);
}

PRESENTER_TEST(
    confirmingAdaptiveCoordinateResponseMeasureTestWithSingleSpeakerHidesTestSetupView) {
    assertHidesTestSetupView(
        confirmingAdaptiveCoordinateResponseMeasureTestWithSingleSpeaker);
}

PRESENTER_TEST(
    confirmingAdaptiveCoordinateResponseMeasureTestWithDelayedMaskerHidesTestSetupView) {
    assertHidesTestSetupView(
        confirmingAdaptiveCoordinateResponseMeasureTestWithDelayedMasker);
}

PRESENTER_TEST(
    confirmingAdaptiveCoordinateResponseMeasureTestWithEyeTrackingHidesTestSetupView) {
    assertHidesTestSetupView(
        confirmingAdaptiveCoordinateResponseMeasureTestWithEyeTracking);
}

PRESENTER_TEST(confirmingAdaptiveOpenSetTestHidesTestSetupView) {
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

PRESENTER_TEST(confirmingAdaptiveOpenSetTestShowsExperimenterView) {
    assertShowsExperimenterView(confirmingAdaptivePassFailTest);
}

PRESENTER_TEST(confirmingAdaptiveOpenSetTestShowsTestingView) {
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
    confirmingDefaultAdaptiveCoordinateResponseMeasureTestShowsSubjectView) {
    assertShowsSubjectView(
        confirmingDefaultAdaptiveCoordinateResponseMeasureTest);
}

PRESENTER_TEST(
    confirmingAdaptiveCoordinateResponseMeasureTestWithSingleSpeakerShowsSubjectView) {
    assertShowsSubjectView(
        confirmingAdaptiveCoordinateResponseMeasureTestWithSingleSpeaker);
}

PRESENTER_TEST(
    confirmingAdaptiveCoordinateResponseMeasureTestWithDelayedMaskerShowsSubjectView) {
    assertShowsSubjectView(
        confirmingAdaptiveCoordinateResponseMeasureTestWithDelayedMasker);
}

PRESENTER_TEST(
    confirmingAdaptiveCoordinateResponseMeasureTestWithEyeTrackingShowsSubjectView) {
    assertShowsSubjectView(
        confirmingAdaptiveCoordinateResponseMeasureTestWithEyeTracking);
}

PRESENTER_TEST(
    confirmingDefaultAdaptiveCoordinateResponseMeasureTestDoesNotShowSubjectViewWhenTestComplete) {
    setTestComplete();
    assertDoesNotShowSubjectView(
        confirmingDefaultAdaptiveCoordinateResponseMeasureTest);
}

PRESENTER_TEST(
    confirmingDefaultAdaptiveCoordinateResponseMeasureTestDoesNotHideSetupViewWhenTestComplete) {
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

PRESENTER_TEST(confirmingAdaptiveOpenSetTestDoesNotShowSubjectView) {
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
    confirmingDefaultAdaptiveCoordinateResponseMeasureTestShowsExperimenterView) {
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
    confirmingAdaptiveCoordinateResponseMeasureTestWithSingleSpeakerInitializesModel) {
    run(confirmingAdaptiveCoordinateResponseMeasureTestWithSingleSpeaker);
    assertTrue(model.initializedWithSingleSpeaker());
}

PRESENTER_TEST(
    confirmingAdaptiveCoordinateResponseMeasureTestWithDelayedMaskerInitializesModel) {
    run(confirmingAdaptiveCoordinateResponseMeasureTestWithDelayedMasker);
    assertTrue(model.initializedWithDelayedMasker());
}

PRESENTER_TEST(
    confirmingAdaptiveCoordinateResponseMeasureTestWithEyeTrackingInitializesModel) {
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
    confirmingDefaultAdaptiveCoordinateResponseMeasureTestDoesNotInitializeFixedLevelTest) {
    assertDoesNotInitializeFixedLevelTest(
        confirmingDefaultAdaptiveCoordinateResponseMeasureTest);
}

PRESENTER_TEST(confirmingAdaptiveOpenSetTestDoesNotInitializeFixedLevelTest) {
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

PRESENTER_TEST(
    confirmingAdaptiveCoordinateResponseMeasureTestPassesStartingSnr) {
    assertStartingSnrPassedToModel(
        confirmingDefaultAdaptiveCoordinateResponseMeasureTest);
}

PRESENTER_TEST(confirmingAdaptiveOpenSetTestPassesStartingSnr) {
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

PRESENTER_TEST(
    confirmingAdaptiveCoordinateResponseMeasureTestPassesMaskerLevel) {
    assertMaskerLevelPassedToModel(
        confirmingDefaultAdaptiveCoordinateResponseMeasureTest);
}

PRESENTER_TEST(confirmingAdaptiveOpenSetTestPassesMaskerLevel) {
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

PRESENTER_TEST(
    confirmingAdaptiveCoordinateResponseMeasureTestPassesTargetList) {
    assertPassesTargetListDirectory(
        confirmingDefaultAdaptiveCoordinateResponseMeasureTest);
}

PRESENTER_TEST(confirmingAdaptiveOpenSetTestPassesTargetList) {
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

PRESENTER_TEST(confirmingAdaptiveCoordinateResponseMeasureTestPassesSubjectId) {
    assertPassesSubjectId(
        confirmingDefaultAdaptiveCoordinateResponseMeasureTest);
}

PRESENTER_TEST(confirmingAdaptiveOpenSetTestPassesSubjectId) {
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

PRESENTER_TEST(confirmingAdaptiveCoordinateResponseMeasureTestPassesTesterId) {
    assertPassesTesterId(
        confirmingDefaultAdaptiveCoordinateResponseMeasureTest);
}

PRESENTER_TEST(confirmingAdaptiveOpenSetTestPassesTesterId) {
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

PRESENTER_TEST(confirmingAdaptiveCoordinateResponseMeasureTestPassesMasker) {
    assertPassesMasker(confirmingDefaultAdaptiveCoordinateResponseMeasureTest);
}

PRESENTER_TEST(confirmingAdaptiveOpenSetTestPassesMasker) {
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

PRESENTER_TEST(confirmingAdaptiveCoordinateResponseMeasureTestPassesSession) {
    assertPassesSession(confirmingDefaultAdaptiveCoordinateResponseMeasureTest);
}

PRESENTER_TEST(confirmingAdaptiveOpenSetTestPassesSession) {
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

PRESENTER_TEST(confirmingAdaptiveCoordinateResponseMeasureTestPassesMethod) {
    assertPassesMethod(confirmingDefaultAdaptiveCoordinateResponseMeasureTest);
}

PRESENTER_TEST(confirmingAdaptiveOpenSetTestPassesMethod) {
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
    confirmingAdaptiveCoordinateResponseMeasureTestWithDelayedMaskerPassesMethod) {
    assertPassesMethod(
        confirmingAdaptiveCoordinateResponseMeasureTestWithDelayedMasker);
}

PRESENTER_TEST(
    confirmingAdaptiveCoordinateResponseMeasureTestWithEyeTrackingPassesMethod) {
    assertPassesMethod(
        confirmingAdaptiveCoordinateResponseMeasureTestWithEyeTracking);
}

PRESENTER_TEST(
    confirmingAdaptiveCoordinateResponseMeasureSingleSpeakerTestPassesMethod) {
    assertPassesMethod(
        confirmingAdaptiveCoordinateResponseMeasureTestWithSingleSpeaker);
}

PRESENTER_TEST(
    confirmingAdaptiveCoordinateResponseMeasureTestPassesCeilingSNR) {
    assertPassesCeilingSNR(
        confirmingDefaultAdaptiveCoordinateResponseMeasureTest);
}

PRESENTER_TEST(confirmingAdaptiveOpenSetTestPassesCeilingSNR) {
    assertPassesCeilingSNR(confirmingAdaptivePassFailTest);
}

PRESENTER_TEST(confirmingAdaptiveCoordinateResponseMeasureTestPassesFloorSNR) {
    assertPassesFloorSNR(
        confirmingDefaultAdaptiveCoordinateResponseMeasureTest);
}

PRESENTER_TEST(confirmingAdaptiveOpenSetTestPassesFloorSNR) {
    assertPassesFloorSNR(confirmingAdaptivePassFailTest);
}

PRESENTER_TEST(
    confirmingAdaptiveCoordinateResponseMeasureTestPassesTrackBumpLimit) {
    assertPassesTrackBumpLimit(
        confirmingDefaultAdaptiveCoordinateResponseMeasureTest);
}

PRESENTER_TEST(confirmingAdaptiveOpenSetTestPassesTrackBumpLimit) {
    assertPassesTrackBumpLimit(confirmingAdaptivePassFailTest);
}

PRESENTER_TEST(
    confirmingAdaptiveCoordinateResponseMeasureTestPassesFullScaleLevel) {
    assertPassesFullScaleLevel(
        confirmingDefaultAdaptiveCoordinateResponseMeasureTest);
}

PRESENTER_TEST(confirmingAdaptiveOpenSetTestPassesFullScaleLevel) {
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
    confirmingAdaptiveCoordinateResponseMeasureTestPassesTrackSettingsFile) {
    assertPassesTrackSettingsFile(
        confirmingDefaultAdaptiveCoordinateResponseMeasureTest);
}

PRESENTER_TEST(confirmingAdaptiveOpenSetTestPassesTrackSettingsFile) {
    assertPassesTrackSettingsFile(confirmingAdaptivePassFailTest);
}

PRESENTER_TEST(
    confirmingAdaptiveCoordinateResponseMeasureTestPassesAudioVisualCondition) {
    assertAudioVisualConditionPassedToModel(
        confirmingDefaultAdaptiveCoordinateResponseMeasureTest);
}

PRESENTER_TEST(
    confirmingAdaptiveCoordinateResponseMeasureTestPassesAuditoryOnlyCondition) {
    assertAuditoryOnlyConditionPassedToModel(
        confirmingDefaultAdaptiveCoordinateResponseMeasureTest);
}

PRESENTER_TEST(confirmingAdaptiveOpenSetTestPassesAudioVisualCondition) {
    assertAudioVisualConditionPassedToModel(confirmingAdaptivePassFailTest);
}

PRESENTER_TEST(confirmingAdaptiveOpenSetTestPassesAuditoryOnlyCondition) {
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
    confirmingDefaultAdaptiveCoordinateResponseMeasureTestShowsNextTrialButtonForSubject) {
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

PRESENTER_TEST(
    confirmingAdaptiveOpenSetTestShowsNextTrialButtonForExperimenter) {
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

PRESENTER_TEST(confirmingAdaptiveOpenSetTestShowsTrialNumber) {
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
    confirmingDefaultAdaptiveCoordinateResponseMeasureTestShowsTrialNumber) {
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
    confirmingAdaptiveCoordinateResponseMeasureTestWithInvalidSnrShowsErrorMessage) {
    assertInvalidSnrShowsErrorMessage(
        confirmingDefaultAdaptiveCoordinateResponseMeasureTest);
}

PRESENTER_TEST(confirmingAdaptiveOpenSetTestWithInvalidSnrShowsErrorMessage) {
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
    confirmingAdaptiveCoordinateResponseMeasureTestWithInvalidInputDoesNotHideSetupView) {
    assertSetupViewNotHiddenWhenSnrIsInvalid(
        confirmingDefaultAdaptiveCoordinateResponseMeasureTest);
}

PRESENTER_TEST(
    confirmingAdaptiveOpenSetTestWithInvalidInputDoesNotHideSetupView) {
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
