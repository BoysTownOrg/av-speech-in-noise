#ifndef AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_METHOD_HPP_
#define AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_METHOD_HPP_

#include <av-speech-in-noise/Interface.hpp>

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

class SessionController {
  public:
    class Observer {
      public:
        AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(Observer);
        virtual void notifyThatTestIsComplete() = 0;
    };
    AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(SessionController);
    virtual void notifyThatTestIsComplete() = 0;
    virtual void prepare(Method) = 0;
    virtual void attach(Observer *) {}
};
}

#endif
