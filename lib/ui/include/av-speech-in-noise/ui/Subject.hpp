#ifndef AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_SUBJECT_HPP_
#define AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_SUBJECT_HPP_

#include <av-speech-in-noise/Interface.hpp>

namespace av_speech_in_noise {
class SubjectView {
  public:
    AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(SubjectView);
    virtual void moveToScreen(int index) = 0;
};
}

#endif
