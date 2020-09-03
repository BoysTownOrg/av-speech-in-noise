#ifndef AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_TESTSETUP_HPP_
#define AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_TESTSETUP_HPP_

#include "PresenterSimple.hpp"
#include <string>

namespace av_speech_in_noise {
enum class Method {
    adaptivePassFail,
    adaptivePassFailWithEyeTracking,
    adaptiveCorrectKeywords,
    adaptiveCorrectKeywordsWithEyeTracking,
    adaptiveCoordinateResponseMeasure,
    adaptiveCoordinateResponseMeasureWithSingleSpeaker,
    adaptiveCoordinateResponseMeasureWithDelayedMasker,
    adaptiveCoordinateResponseMeasureWithEyeTracking,
    fixedLevelFreeResponseWithTargetReplacement,
    fixedLevelFreeResponseWithSilentIntervalTargets,
    fixedLevelFreeResponseWithAllTargets,
    fixedLevelFreeResponseWithAllTargetsAndEyeTracking,
    fixedLevelCoordinateResponseMeasureWithTargetReplacement,
    fixedLevelCoordinateResponseMeasureWithTargetReplacementAndEyeTracking,
    fixedLevelCoordinateResponseMeasureWithSilentIntervalTargets,
    fixedLevelConsonants,
    unknown
};

class SomethingIDK {
  public:
    virtual ~SomethingIDK() = default;
    virtual void prepare(Method) = 0;
};

class TestSetupResponder {
  public:
    class EventListener {
      public:
        virtual ~EventListener() = default;
        virtual void notifyThatUserHasSelectedTestSettingsFile(
            const std::string &) = 0;
    };
    virtual ~TestSetupResponder() = default;
    virtual void becomeChild(SomethingIDK *) = 0;
    virtual void subscribe(EventListener *) = 0;
};

class TestSetupPresenter : public virtual TestSetupResponder::EventListener,
                           public virtual PresenterSimple {};
}

#endif
