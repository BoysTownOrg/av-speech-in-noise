#ifndef AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_PRESENTER_HPP_
#define AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_PRESENTER_HPP_

namespace av_speech_in_noise {
class Presenter {
  public:
    virtual ~Presenter() = default;
    virtual void start() = 0;
    virtual void stop() = 0;
};
}

#endif
