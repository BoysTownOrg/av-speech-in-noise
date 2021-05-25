#ifndef AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_EYETRACKERCALIBRATION_HPP_
#define AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_EYETRACKERCALIBRATION_HPP_

#include <av-speech-in-noise/Interface.hpp>
#include <av-speech-in-noise/core/EyeTrackerCalibration.hpp>
#include <algorithm>
#include <cmath>
#include <sstream>
#include <string>

namespace av_speech_in_noise::eye_tracking::calibration {
struct WindowPoint {
    double x;
    double y;
};

struct Line {
    WindowPoint a;
    WindowPoint b;
};

class View {
  public:
    class Observer {
      public:
        AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(Observer);
        virtual void notifyThatAnimationHasFinished() = 0;
    };
    AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(View);
    virtual void attach(Observer *) = 0;
    virtual void moveDotTo(WindowPoint) = 0;
    virtual void shrinkDot() = 0;
    virtual void growDot() = 0;
    virtual void drawRed(Line) = 0;
    virtual void drawGreen(Line) = 0;
    virtual void drawWhiteCircleWithCenter(WindowPoint) = 0;
    virtual void clear() = 0;
    virtual void show() = 0;
    virtual void hide() = 0;
};

class Presenter : public View::Observer,
                  public SubjectPresenter,
                  public TesterPresenter {
  public:
    enum class DotState { idle, moving, shrinking, shrunk, growing };
    explicit Presenter(View &view) : view{view} { view.attach(this); }
    void attach(SubjectPresenter::Observer *a) override { observer = a; }
    void present(Point x) override;
    void present(const std::vector<Result> &) override;
    void notifyThatAnimationHasFinished() override;
    void stop() override;
    void start() override;

  private:
    Point pointPresenting{};
    View &view;
    SubjectPresenter::Observer *observer{};
    DotState dotState{DotState::idle};
};

class Control {
  public:
    class Observer {
      public:
        AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(Observer);
        virtual void notifyThatWindowHasBeenTouched(WindowPoint) = 0;
        virtual void notifyThatSubmitButtonHasBeenClicked() = 0;
        virtual void notifyThatMenuHasBeenSelected() = 0;
    };
    AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(Control);
    virtual void attach(Observer *) = 0;
    virtual auto whiteCircleCenters() -> std::vector<WindowPoint> = 0;
    virtual auto whiteCircleDiameter() -> double = 0;
};

class Controller : public Control::Observer {
  public:
    Controller(Control &, IInteractor &);
    void notifyThatWindowHasBeenTouched(WindowPoint) override;
    void notifyThatSubmitButtonHasBeenClicked() override;
    void notifyThatMenuHasBeenSelected() override;

  private:
    IInteractor &interactor;
    Control &control;
};

namespace validation {
class TesterView {
  public:
    AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(TesterView);
    virtual void setLeftEyeAccuracyDegrees(const std::string &s) = 0;
    virtual void setLeftEyePrecisionDegrees(const std::string &s) = 0;
    virtual void setRightEyeAccuracyDegrees(const std::string &s) = 0;
    virtual void setRightEyePrecisionDegrees(const std::string &s) = 0;
};

static auto format(float x) -> std::string {
    std::stringstream stream;
    stream << x;
    return stream.str();
}

class TesterPresenterImpl : public TesterPresenter {
  public:
    explicit TesterPresenterImpl(TesterView &view) : view{view} {}

    void present(const Result &result) override {
        view.setLeftEyeAccuracyDegrees(
            format(result.left.errorOfMeanGaze.degrees));
        view.setLeftEyePrecisionDegrees(
            format(result.left.standardDeviationFromTheMeanGaze.degrees));
        view.setRightEyeAccuracyDegrees(
            format(result.right.errorOfMeanGaze.degrees));
        view.setRightEyePrecisionDegrees(
            format(result.right.standardDeviationFromTheMeanGaze.degrees));
    }
    void start() override {}
    void stop() override {}

  private:
    TesterView &view;
};
}
}

#endif
