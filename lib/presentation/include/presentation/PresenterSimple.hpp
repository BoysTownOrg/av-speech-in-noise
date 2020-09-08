#ifndef AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_PRESENTERSIMPLE_HPP_
#define AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_PRESENTERSIMPLE_HPP_

class PresenterSimple {
  public:
    virtual ~PresenterSimple() = default;
    virtual void start() = 0;
    virtual void stop() = 0;
};

#endif
