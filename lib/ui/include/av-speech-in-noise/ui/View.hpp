#ifndef AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_VIEW_HPP_
#define AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_VIEW_HPP_

#include <av-speech-in-noise/Interface.hpp>

namespace av_speech_in_noise {
class View {
  public:
    AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(View);
    virtual void show() = 0;
    virtual void hide() = 0;
};
}

#endif
