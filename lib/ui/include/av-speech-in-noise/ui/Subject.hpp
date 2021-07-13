#ifndef AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_SUBJECT_HPP_
#define AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_SUBJECT_HPP_

#include "Presenter.hpp"
#include "View.hpp"

#include <av-speech-in-noise/Interface.hpp>

namespace av_speech_in_noise {
class SubjectView : public View {
  public:
    virtual void moveToScreen(int index) = 0;
};

class SubjectPresenter : public Presenter {};
}

#endif
