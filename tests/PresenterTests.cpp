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
        Method::fixedLevelFreeResponseWithTargetReplacement);
    assertSetupViewMethodsContains(
        Method::fixedLevelFreeResponseWithAllTargets);
    assertSetupViewMethodsContains(
        Method::fixedLevelFreeResponseWithSilentIntervalTargets);
    assertSetupViewMethodsContains(
        Method::fixedLevelCoordinateResponseMeasureWithTargetReplacement);
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

PRESENTER_TEST(confirmingAdaptivePassFailTestHidesTestSetupView) {
    assertHidesTestSetupView(confirmingAdaptivePassFailTest);
}

PRESENTER_TEST(
    confirmingFixedLevelFreeResponseTestWithTargetReplacementHidesTestSetupView) {
    assertHidesTestSetupView(
        confirmingFixedLevelFreeResponseWithTargetReplacementTest);
}

PRESENTER_TEST(
    confirmingFixedLevelCoordinateResponseMeasureTestWithTargetReplacementHidesTestSetupView) {
    assertHidesTestSetupView(
        confirmingFixedLevelCoordinateResponseMeasureWithTargetReplacementTest);
}

PRESENTER_TEST(
    confirmingFixedLevelCoordinateResponseMeasureTestWithSilentIntervalTargetsHidesTestSetupView) {
    assertHidesTestSetupView(
        confirmingFixedLevelCoordinateResponseMeasureSilentIntervalsTest);
}

PRESENTER_TEST(confirmingAdaptivePassFailTestShowsExperimenterView) {
    assertShowsExperimenterView(confirmingAdaptivePassFailTest);
}

PRESENTER_TEST(confirmingAdaptivePassFailTestShowsTestingView) {
    assertShowsTestingView(confirmingAdaptivePassFailTest);
}

PRESENTER_TEST(
    confirmingFixedLevelFreeResponseTestWithTargetReplacementShowsTestingView) {
    assertShowsTestingView(
        confirmingFixedLevelFreeResponseWithTargetReplacementTest);
}

PRESENTER_TEST(
    confirmingFixedLevelFreeResponseTestWithTargetReplacementShowsExperimenterView) {
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

PRESENTER_TEST(
    confirmingFixedLevelCoordinateResponseMeasureTestWithTargetReplacementShowsSubjectView) {
    assertShowsSubjectView(
        confirmingFixedLevelCoordinateResponseMeasureWithTargetReplacementTest);
}

PRESENTER_TEST(
    confirmingFixedLevelCoordinateResponseMeasureTestWithSilentIntervalTargetsShowsSubjectView) {
    assertShowsSubjectView(
        confirmingFixedLevelCoordinateResponseMeasureSilentIntervalsTest);
}

PRESENTER_TEST(confirmingAdaptivePassFailTestDoesNotShowSubjectView) {
    assertDoesNotShowSubjectView(confirmingAdaptivePassFailTest);
}

PRESENTER_TEST(
    confirmingFixedLevelFreeResponseTestWithTargetReplacementDoesNotShowSubjectView) {
    assertDoesNotShowSubjectView(
        confirmingFixedLevelFreeResponseWithTargetReplacementTest);
}

PRESENTER_TEST(
    confirmingFixedLevelFreeResponseTestWithAllTargetsDoesNotShowSubjectView) {
    assertDoesNotShowSubjectView(
        confirmingFixedLevelFreeResponseTestWithAllTargets);
}

PRESENTER_TEST(
    confirmingFixedLevelFreeResponseTestWithSilentIntervalTargetsDoesNotShowSubjectView) {
    assertDoesNotShowSubjectView(
        confirmingFixedLevelFreeResponseWithSilentIntervalTargetsTest);
}

PRESENTER_TEST(
    confirmingDefaultAdaptiveCoordinateResponseMeasureTestShowsExperimenterView) {
    assertShowsExperimenterView(
        confirmingDefaultAdaptiveCoordinateResponseMeasureTest);
}

PRESENTER_TEST(
    confirmingFixedLevelCoordinateResponseMeasureTestWithTargetReplacementShowsExperimenterView) {
    assertShowsExperimenterView(
        confirmingFixedLevelCoordinateResponseMeasureWithTargetReplacementTest);
}

PRESENTER_TEST(
    confirmingFixedLevelCoordinateResponseMeasureTestWithSilentIntervalTargetsShowsExperimenterView) {
    assertShowsExperimenterView(
        confirmingFixedLevelCoordinateResponseMeasureSilentIntervalsTest);
}

PRESENTER_TEST(
    confirmingDefaultAdaptiveCoordinateResponseMeasureTestDoesNotInitializeFixedLevelTest) {
    assertDoesNotInitializeFixedLevelTest(
        confirmingDefaultAdaptiveCoordinateResponseMeasureTest);
}

PRESENTER_TEST(confirmingAdaptivePassFailTestDoesNotInitializeFixedLevelTest) {
    assertDoesNotInitializeFixedLevelTest(confirmingAdaptivePassFailTest);
}

PRESENTER_TEST(
    confirmingFixedLevelFreeResponseTestWithTargetReplacementDoesNotInitializeAdaptiveTest) {
    assertDoesNotInitializeAdaptiveTest(
        confirmingFixedLevelFreeResponseWithTargetReplacementTest);
}

PRESENTER_TEST(
    confirmingFixedLevelFreeResponseTestWithAllTargetsDoesNotInitializeAdaptiveTest) {
    assertDoesNotInitializeAdaptiveTest(
        confirmingFixedLevelFreeResponseTestWithAllTargets);
}

PRESENTER_TEST(
    confirmingFixedLevelFreeResponseTestWithSilentIntervalTargetsDoesNotInitializeAdaptiveTest) {
    assertDoesNotInitializeAdaptiveTest(
        confirmingFixedLevelFreeResponseWithSilentIntervalTargetsTest);
}

PRESENTER_TEST(
    confirmingFixedLevelCoordinateResponseMeasureTestWithTargetReplacementDoesNotInitializeAdaptiveTest) {
    assertDoesNotInitializeAdaptiveTest(
        confirmingFixedLevelCoordinateResponseMeasureWithTargetReplacementTest);
}

PRESENTER_TEST(
    confirmingFixedLevelCoordinateResponseMeasureTestWithSilentIntervalTargetsDoesNotInitializeAdaptiveTest) {
    assertDoesNotInitializeAdaptiveTest(
        confirmingFixedLevelCoordinateResponseMeasureSilentIntervalsTest);
}

PRESENTER_TEST(
    confirmingAdaptiveCoordinateResponseMeasureTestPassesStartingSnr) {
    assertStartingSnrPassedToModel(
        confirmingDefaultAdaptiveCoordinateResponseMeasureTest);
}

PRESENTER_TEST(confirmingAdaptivePassFailTestPassesStartingSnr) {
    assertStartingSnrPassedToModel(confirmingAdaptivePassFailTest);
}

PRESENTER_TEST(
    confirmingFixedLevelFreeResponseTestWithTargetReplacementPassesStartingSnr) {
    assertStartingSnrPassedToModel(
        confirmingFixedLevelFreeResponseWithTargetReplacementTest);
}

PRESENTER_TEST(
    confirmingFixedLevelCoordinateResponseMeasureTestWithTargetReplacementPassesStartingSnr) {
    assertStartingSnrPassedToModel(
        confirmingFixedLevelCoordinateResponseMeasureWithTargetReplacementTest);
}

PRESENTER_TEST(
    confirmingFixedLevelCoordinateResponseMeasureTestWithSilentIntervalTargetsPassesStartingSnr) {
    assertStartingSnrPassedToModel(
        confirmingFixedLevelCoordinateResponseMeasureSilentIntervalsTest);
}

PRESENTER_TEST(
    confirmingAdaptiveCoordinateResponseMeasureTestPassesMaskerLevel) {
    assertMaskerLevelPassedToModel(
        confirmingDefaultAdaptiveCoordinateResponseMeasureTest);
}

PRESENTER_TEST(confirmingAdaptivePassFailTestPassesMaskerLevel) {
    assertMaskerLevelPassedToModel(confirmingAdaptivePassFailTest);
}

PRESENTER_TEST(
    confirmingFixedLevelFreeResponseTestWithTargetReplacementPassesMaskerLevel) {
    assertMaskerLevelPassedToModel(
        confirmingFixedLevelFreeResponseWithTargetReplacementTest);
}

PRESENTER_TEST(
    confirmingFixedLevelCoordinateResponseMeasureTestWithTargetReplacementPassesMaskerLevel) {
    assertMaskerLevelPassedToModel(
        confirmingFixedLevelCoordinateResponseMeasureWithTargetReplacementTest);
}

PRESENTER_TEST(playCalibrationPassesLevel) {
    setCalibrationLevel("1");
    playCalibration();
    assertEqual(1, calibration().level_dB_SPL);
}

PRESENTER_TEST(
    confirmingAdaptiveCoordinateResponseMeasureTestPassesTestSettingsFileToTextFileReader) {
    assertPassesTestSettingsFileToTextFileReader(
        confirmingDefaultAdaptiveCoordinateResponseMeasureTest);
}

PRESENTER_TEST(
    confirmingAdaptiveCoordinateResponseMeasureTestPassesTestSettingsTextToTestSettingsInterpreterForMethodQuery) {
    assertPassesTestSettingsTextToTestSettingsInterpreterForMethodQuery(
        confirmingDefaultAdaptiveCoordinateResponseMeasureTest);
}

PRESENTER_TEST(
    confirmingAdaptiveCoordinateResponseMeasureTestPassesTestSettingsTextToTestSettingsInterpreter) {
    assertPassesTestSettingsTextToTestSettingsInterpreter(
        confirmingDefaultAdaptiveCoordinateResponseMeasureTest);
}

PRESENTER_TEST(
    confirmingAdaptiveCoordinateResponseMeasureTestPassesTargetList) {
    assertPassesTargetListDirectory(
        confirmingDefaultAdaptiveCoordinateResponseMeasureTest);
}

PRESENTER_TEST(confirmingAdaptivePassFailTestPassesTargetList) {
    assertPassesTargetListDirectory(confirmingAdaptivePassFailTest);
}

PRESENTER_TEST(
    confirmingFixedLevelFreeResponseTestWithTargetReplacementPassesTargetList) {
    assertPassesTargetListDirectory(
        confirmingFixedLevelFreeResponseWithTargetReplacementTest);
}

PRESENTER_TEST(
    confirmingFixedLevelCoordinateResponseMeasureTestWithTargetReplacementPassesTargetList) {
    assertPassesTargetListDirectory(
        confirmingFixedLevelCoordinateResponseMeasureWithTargetReplacementTest);
}

PRESENTER_TEST(confirmingAdaptiveCoordinateResponseMeasureTestPassesSubjectId) {
    assertPassesSubjectId(
        confirmingDefaultAdaptiveCoordinateResponseMeasureTest);
}

PRESENTER_TEST(confirmingAdaptivePassFailTestPassesSubjectId) {
    assertPassesSubjectId(confirmingAdaptivePassFailTest);
}

PRESENTER_TEST(
    confirmingFixedLevelFreeResponseTestWithTargetReplacementPassesSubjectId) {
    assertPassesSubjectId(
        confirmingFixedLevelFreeResponseWithTargetReplacementTest);
}

PRESENTER_TEST(
    confirmingFixedLevelCoordinateResponseMeasureTestWithTargetReplacementPassesSubjectId) {
    assertPassesSubjectId(
        confirmingFixedLevelCoordinateResponseMeasureWithTargetReplacementTest);
}

PRESENTER_TEST(confirmingAdaptiveCoordinateResponseMeasureTestPassesTesterId) {
    assertPassesTesterId(
        confirmingDefaultAdaptiveCoordinateResponseMeasureTest);
}

PRESENTER_TEST(confirmingAdaptivePassFailTestPassesTesterId) {
    assertPassesTesterId(confirmingAdaptivePassFailTest);
}

PRESENTER_TEST(
    confirmingFixedLevelFreeResponseTestWithTargetReplacementPassesTesterId) {
    assertPassesTesterId(
        confirmingFixedLevelFreeResponseWithTargetReplacementTest);
}

PRESENTER_TEST(
    confirmingFixedLevelCoordinateResponseMeasureTestWithTargetReplacementPassesTesterId) {
    assertPassesTesterId(
        confirmingFixedLevelCoordinateResponseMeasureWithTargetReplacementTest);
}

PRESENTER_TEST(confirmingAdaptiveCoordinateResponseMeasureTestPassesMasker) {
    assertPassesMasker(confirmingDefaultAdaptiveCoordinateResponseMeasureTest);
}

PRESENTER_TEST(confirmingAdaptivePassFailTestPassesMasker) {
    assertPassesMasker(confirmingAdaptivePassFailTest);
}

PRESENTER_TEST(
    confirmingFixedLevelFreeResponseTestWithTargetReplacementPassesMasker) {
    assertPassesMasker(
        confirmingFixedLevelFreeResponseWithTargetReplacementTest);
}

PRESENTER_TEST(
    confirmingFixedLevelCoordinateResponseMeasureTestWithTargetReplacementPassesMasker) {
    assertPassesMasker(
        confirmingFixedLevelCoordinateResponseMeasureWithTargetReplacementTest);
}

PRESENTER_TEST(playCalibrationPassesFilePath) {
    setupView.setCalibrationFilePath("a");
    playCalibration();
    assertEqual("a", calibration().filePath);
}

PRESENTER_TEST(confirmingAdaptiveCoordinateResponseMeasureTestPassesSession) {
    assertPassesSession(confirmingDefaultAdaptiveCoordinateResponseMeasureTest);
}

PRESENTER_TEST(confirmingAdaptivePassFailTestPassesSession) {
    assertPassesSession(confirmingAdaptivePassFailTest);
}

PRESENTER_TEST(
    confirmingFixedLevelFreeResponseTestWithTargetReplacementPassesSession) {
    assertPassesSession(
        confirmingFixedLevelFreeResponseWithTargetReplacementTest);
}

PRESENTER_TEST(
    confirmingFixedLevelCoordinateResponseMeasureTestWithTargetReplacementPassesSession) {
    assertPassesSession(
        confirmingFixedLevelCoordinateResponseMeasureWithTargetReplacementTest);
}

PRESENTER_TEST(confirmingAdaptiveCoordinateResponseMeasureTestPassesMethod) {
    assertPassesMethod(confirmingDefaultAdaptiveCoordinateResponseMeasureTest);
}

PRESENTER_TEST(confirmingAdaptivePassFailTestPassesMethod) {
    assertPassesMethod(confirmingAdaptivePassFailTest);
}

PRESENTER_TEST(
    confirmingFixedLevelFreeResponseTestWithTargetReplacementPassesMethod) {
    assertPassesMethod(
        confirmingFixedLevelFreeResponseWithTargetReplacementTest);
}

PRESENTER_TEST(
    confirmingFixedLevelCoordinateResponseMeasureTestWithTargetReplacementPassesMethod) {
    assertPassesMethod(
        confirmingFixedLevelCoordinateResponseMeasureWithTargetReplacementTest);
}

PRESENTER_TEST(
    confirmingAdaptiveCoordinateResponseMeasureTestPassesCeilingSNR) {
    assertPassesCeilingSNR(
        confirmingDefaultAdaptiveCoordinateResponseMeasureTest);
}

PRESENTER_TEST(confirmingAdaptivePassFailTestPassesCeilingSNR) {
    assertPassesCeilingSNR(confirmingAdaptivePassFailTest);
}

PRESENTER_TEST(confirmingAdaptiveCoordinateResponseMeasureTestPassesFloorSNR) {
    assertPassesFloorSNR(
        confirmingDefaultAdaptiveCoordinateResponseMeasureTest);
}

PRESENTER_TEST(confirmingAdaptivePassFailTestPassesFloorSNR) {
    assertPassesFloorSNR(confirmingAdaptivePassFailTest);
}

PRESENTER_TEST(
    confirmingAdaptiveCoordinateResponseMeasureTestPassesTrackBumpLimit) {
    assertPassesTrackBumpLimit(
        confirmingDefaultAdaptiveCoordinateResponseMeasureTest);
}

PRESENTER_TEST(confirmingAdaptivePassFailTestPassesTrackBumpLimit) {
    assertPassesTrackBumpLimit(confirmingAdaptivePassFailTest);
}

PRESENTER_TEST(
    confirmingAdaptiveCoordinateResponseMeasureTestPassesFullScaleLevel) {
    assertPassesFullScaleLevel(
        confirmingDefaultAdaptiveCoordinateResponseMeasureTest);
}

PRESENTER_TEST(confirmingAdaptivePassFailTestPassesFullScaleLevel) {
    assertPassesFullScaleLevel(confirmingAdaptivePassFailTest);
}

PRESENTER_TEST(
    confirmingFixedLevelFreeResponseTestWithTargetReplacementPassesFullScaleLevel) {
    assertPassesFullScaleLevel(
        confirmingFixedLevelFreeResponseWithTargetReplacementTest);
}

PRESENTER_TEST(
    confirmingFixedLevelCoordinateResponseMeasureTestWithTargetReplacementPassesFullScaleLevel) {
    assertPassesFullScaleLevel(
        confirmingFixedLevelCoordinateResponseMeasureWithTargetReplacementTest);
}

PRESENTER_TEST(playCalibrationPassesFullScaleLevel) {
    assertPassesFullScaleLevel(playingCalibration);
}

PRESENTER_TEST(exitTestAfterCompletingTrialHidesCorrectKeywordsSubmission) {
    assertExitTestAfterCompletingTrialHidesResponseSubmission(
        confirmingAdaptiveCorrectKeywordsTest, submittingCorrectKeywords);
}

PRESENTER_TEST(exitTestAfterCompletingTrialHidesFreeResponseSubmission) {
    assertExitTestAfterCompletingTrialHidesResponseSubmission(
        confirmingFixedLevelFreeResponseTestWithAllTargets,
        submittingFreeResponse);
}

PRESENTER_TEST(exitTestAfterCompletingTrialHidesPassFailSubmission) {
    assertExitTestAfterCompletingTrialHidesResponseSubmission(
        confirmingAdaptivePassFailTest, submittingPassedTrial);
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

PRESENTER_TEST(confirmingAdaptivePassFailTestPassesAudioVisualCondition) {
    assertAudioVisualConditionPassedToModel(confirmingAdaptivePassFailTest);
}

PRESENTER_TEST(confirmingAdaptivePassFailTestPassesAuditoryOnlyCondition) {
    assertAuditoryOnlyConditionPassedToModel(confirmingAdaptivePassFailTest);
}

PRESENTER_TEST(
    confirmingFixedLevelFreeResponseTestWithTargetReplacementPassesAudioVisualCondition) {
    assertAudioVisualConditionPassedToModel(
        confirmingFixedLevelFreeResponseWithTargetReplacementTest);
}

PRESENTER_TEST(
    confirmingFixedLevelFreeResponseTestWithTargetReplacementPassesAuditoryOnlyCondition) {
    assertAuditoryOnlyConditionPassedToModel(
        confirmingFixedLevelFreeResponseWithTargetReplacementTest);
}

PRESENTER_TEST(
    confirmingFixedLevelCoordinateResponseMeasureTestWithTargetReplacementPassesAudioVisualCondition) {
    assertAudioVisualConditionPassedToModel(
        confirmingFixedLevelCoordinateResponseMeasureWithTargetReplacementTest);
}

PRESENTER_TEST(
    confirmingFixedLevelCoordinateResponseMeasureTestWithTargetReplacementPassesAuditoryOnlyCondition) {
    assertAuditoryOnlyConditionPassedToModel(
        confirmingFixedLevelCoordinateResponseMeasureWithTargetReplacementTest);
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
    confirmingFixedLevelCoordinateResponseMeasureTestWithTargetReplacementShowsNextTrialButtonForSubject) {
    assertConfirmTestSetupShowsNextTrialButton(
        confirmingFixedLevelCoordinateResponseMeasureWithTargetReplacementTest,
        playingTrialFromSubject);
}

PRESENTER_TEST(
    confirmingFixedLevelCoordinateResponseMeasureTestWithSilentIntervalTargetsShowsNextTrialButtonForSubject) {
    assertConfirmTestSetupShowsNextTrialButton(
        confirmingFixedLevelCoordinateResponseMeasureSilentIntervalsTest,
        playingTrialFromSubject);
}

PRESENTER_TEST(
    confirmingAdaptivePassFailTestShowsNextTrialButtonForExperimenter) {
    assertConfirmTestSetupShowsNextTrialButton(
        confirmingAdaptivePassFailTest, playingTrialFromExperimenter);
}

PRESENTER_TEST(
    confirmingFixedLevelFreeResponseTestWithTargetReplacementShowsNextTrialButtonForExperimenter) {
    assertConfirmTestSetupShowsNextTrialButton(
        confirmingFixedLevelFreeResponseWithTargetReplacementTest,
        playingTrialFromExperimenter);
}

PRESENTER_TEST(
    confirmingFixedLevelFreeResponseTestWithAllTargetsShowsNextTrialButtonForExperimenter) {
    assertConfirmTestSetupShowsNextTrialButton(
        confirmingFixedLevelFreeResponseTestWithAllTargets,
        playingTrialFromExperimenter);
}

PRESENTER_TEST(
    confirmingFixedLevelFreeResponseTestWithSilentIntervalTargetsShowsNextTrialButtonForExperimenter) {
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
    confirmingFixedLevelFreeResponseTestWithTargetReplacementWithInvalidMaskerLevelShowsErrorMessage) {
    assertInvalidMaskerLevelShowsErrorMessage(
        confirmingFixedLevelFreeResponseWithTargetReplacementTest);
}

PRESENTER_TEST(
    confirmingFixedLevelCoordinateResponseMeasureTestWithTargetReplacementWithInvalidMaskerLevelShowsErrorMessage) {
    assertInvalidMaskerLevelShowsErrorMessage(
        confirmingFixedLevelCoordinateResponseMeasureWithTargetReplacementTest);
}

PRESENTER_TEST(
    confirmingAdaptiveCoordinateResponseMeasureTestDoesNotShowExperimentersNextTrialButton) {
    run(confirmingDefaultAdaptiveCoordinateResponseMeasureTest);
    assertFalse(experimenterView.nextTrialButtonShown());
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
    experimenterView.setResponse("a");
    respondFromExperimenter();
    assertEqual("a", model.freeResponse().response);
}

PRESENTER_TEST(experimenterResponseFlagsResponse) {
    experimenterView.flagResponse();
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

PRESENTER_TEST(browseForTestSettingsFileUpdatesTestSettingsFile) {
    assertBrowseResultPassedToEntry(browsingForTestSettingsFile);
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

PRESENTER_TEST(browseForTestSettingsCancelDoesNotChangeTestSettingsFile) {
    assertCancellingBrowseDoesNotChangePath(browsingForTestSettingsFile);
}

PRESENTER_TEST(completingTrialShowsExitTestButton) {
    completeTrial();
    assertTrue(exitTestButtonShown());
}

PRESENTER_TEST(
    confirmingFixedLevelFreeResponseTestWithTargetReplacementShowsTrialNumber) {
    assertShowsTrialNumber(
        confirmingFixedLevelFreeResponseWithTargetReplacementTest);
}

PRESENTER_TEST(
    confirmingFixedLevelFreeResponseTestWithAllTargetsShowsTrialNumber) {
    assertShowsTrialNumber(confirmingFixedLevelFreeResponseTestWithAllTargets);
}

PRESENTER_TEST(
    confirmingFixedLevelFreeResponseTestWithSilentIntervalTargetsShowsTrialNumber) {
    assertShowsTrialNumber(
        confirmingFixedLevelFreeResponseWithSilentIntervalTargetsTest);
}

PRESENTER_TEST(confirmingAdaptivePassFailTestShowsTrialNumber) {
    assertShowsTrialNumber(confirmingAdaptivePassFailTest);
}

PRESENTER_TEST(
    confirmingFixedLevelCoordinateResponseMeasureTestWithTargetReplacementShowsTrialNumber) {
    assertShowsTrialNumber(
        confirmingFixedLevelCoordinateResponseMeasureWithTargetReplacementTest);
}

PRESENTER_TEST(
    confirmingFixedLevelCoordinateResponseMeasureTestWithSilentIntervalTargetsShowsTrialNumber) {
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

TEST_F(
    PresenterTests, confirmingAdaptiveCorrectKeywordsTestShowsTargetFileName) {
    assertShowsTargetFileName(confirmingAdaptiveCorrectKeywordsTest);
}

TEST_F(PresenterTests, confirmingAdaptiveClosedSetTestShowsTargetFileName) {
    assertShowsTargetFileName(
        confirmingDefaultAdaptiveCoordinateResponseMeasureTest);
}

TEST_F(PresenterTests, submittingCorrectKeywordsShowsTargetFileName) {
    assertShowsTargetFileName(submittingCorrectKeywords);
}

TEST_F(PresenterTests, submittingCoordinateResponseShowsTargetFileName) {
    assertShowsTargetFileName(respondingFromSubject);
}

TEST_F(PresenterTests, submittingFreeResponseShowsTargetFileName) {
    assertShowsTargetFileName(submittingFreeResponse);
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
    completingTrialShowsSubjectResponseButtonsForFixedLevelCoordinateResponseMeasureWithTargetReplacementTest) {
    assertCompleteTrialShowsResponseView(
        confirmingFixedLevelCoordinateResponseMeasureWithTargetReplacementTest,
        respondingFromSubject);
}

PRESENTER_TEST(
    completingTrialShowsSubjectResponseButtonsForFixedLevelCoordinateResponseMeasureSilentIntervalsTest) {
    assertCompleteTrialShowsResponseView(
        confirmingFixedLevelCoordinateResponseMeasureSilentIntervalsTest,
        respondingFromSubject);
}

PRESENTER_TEST(
    completingTrialShowsExperimenterEvaluationButtonsForAdaptivePassFailTest) {
    assertCompleteTrialShowsResponseView(
        confirmingAdaptivePassFailTest, submittingPassedTrial);
}

PRESENTER_TEST(
    completingTrialShowsExperimenterResponseSubmissionForFixedLevelFreeResponseTest) {
    assertCompleteTrialShowsResponseView(
        confirmingFixedLevelFreeResponseWithTargetReplacementTest,
        submittingFreeResponse);
}

PRESENTER_TEST(
    completingTrialShowsExperimenterResponseSubmissionForFixedLevelFreeResponseAllStimuliTest) {
    assertCompleteTrialShowsResponseView(
        confirmingFixedLevelFreeResponseTestWithAllTargets,
        submittingFreeResponse);
}

PRESENTER_TEST(
    completingTrialShowsExperimenterResponseSubmissionForFixedLevelFreeResponseSilentIntervalsTest) {
    assertCompleteTrialShowsResponseView(
        confirmingFixedLevelFreeResponseWithSilentIntervalTargetsTest,
        submittingFreeResponse);
}

PRESENTER_TEST(
    confirmingAdaptiveCoordinateResponseMeasureTestWithInvalidSnrShowsErrorMessage) {
    assertInvalidSnrShowsErrorMessage(
        confirmingDefaultAdaptiveCoordinateResponseMeasureTest);
}

PRESENTER_TEST(confirmingAdaptivePassFailTestWithInvalidSnrShowsErrorMessage) {
    assertInvalidSnrShowsErrorMessage(confirmingAdaptivePassFailTest);
}

PRESENTER_TEST(
    confirmingFixedLevelFreeResponseTestWithTargetReplacementWithInvalidSnrShowsErrorMessage) {
    assertInvalidSnrShowsErrorMessage(
        confirmingFixedLevelFreeResponseWithTargetReplacementTest);
}

PRESENTER_TEST(
    confirmingFixedLevelCoordinateResponseMeasureTestWithTargetReplacementWithInvalidSnrShowsErrorMessage) {
    assertInvalidSnrShowsErrorMessage(
        confirmingFixedLevelCoordinateResponseMeasureWithTargetReplacementTest);
}

PRESENTER_TEST(
    confirmingAdaptiveCoordinateResponseMeasureTestWithInvalidInputDoesNotHideSetupView) {
    assertSetupViewNotHiddenWhenSnrIsInvalid(
        confirmingDefaultAdaptiveCoordinateResponseMeasureTest);
}

PRESENTER_TEST(
    confirmingAdaptivePassFailTestWithInvalidInputDoesNotHideSetupView) {
    assertSetupViewNotHiddenWhenSnrIsInvalid(confirmingAdaptivePassFailTest);
}

PRESENTER_TEST(
    confirmingFixedLevelFreeResponseTestWithTargetReplacementWithInvalidInputDoesNotHideSetupView) {
    assertSetupViewNotHiddenWhenSnrIsInvalid(
        confirmingFixedLevelFreeResponseWithTargetReplacementTest);
}

PRESENTER_TEST(
    confirmingFixedLevelCoordinateResponseMeasureTestWithTargetReplacementWithInvalidInputDoesNotHideSetupView) {
    assertSetupViewNotHiddenWhenSnrIsInvalid(
        confirmingFixedLevelCoordinateResponseMeasureWithTargetReplacementTest);
}

PRESENTER_TEST(
    confirmingFixedLevelCoordinateResponseMeasureTestWithSilentIntervalTargetsWithInvalidInputDoesNotHideSetupView) {
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
