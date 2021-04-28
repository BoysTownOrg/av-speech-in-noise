#ifndef AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_EYETRACKERCALIBRATION_HPP_
#define AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_EYETRACKERCALIBRATION_HPP_

#include <av-speech-in-noise/Interface.hpp>
#include <recognition-test/EyeTrackerCalibration.hpp>

namespace av_speech_in_noise::eye_tracker_calibration {
class View {
  public:
    class Observer {
      public:
        AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(Observer);
        virtual void notifyThatAnimationHasFinished() = 0;
    };
    AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(View);
    virtual void attach(Observer *) = 0;
    virtual void moveDotTo(Point) = 0;
    virtual void shrinkDot() = 0;
    virtual void growDot() = 0;
};

class Presenter : public View::Observer, public IPresenter {
  public:
    enum class DotState { idle, moving, shrinking, shrunk, growing };

    explicit Presenter(View &view) : view{view} { view.attach(this); }
    void attach(IPresenter::Observer *a) override { observer = a; }
    void present(Point x) override;
    void notifyThatAnimationHasFinished() override;

  private:
    Point pointPresenting{};
    View &view;
    IPresenter::Observer *observer{};
    DotState dotState{DotState::idle};
};
}

#endif
