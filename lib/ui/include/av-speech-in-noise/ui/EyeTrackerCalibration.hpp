#ifndef AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_EYETRACKERCALIBRATION_HPP_
#define AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_EYETRACKERCALIBRATION_HPP_

#include "View.hpp"
#include "Subject.hpp"

#include <av-speech-in-noise/core/EyeTrackerCalibration.hpp>
#include <av-speech-in-noise/core/ITimer.hpp>
#include <av-speech-in-noise/Interface.hpp>

#include <string>

namespace av_speech_in_noise::eye_tracker_calibration {
struct WindowPoint {
    double x;
    double y;
};

struct Line {
    WindowPoint a;
    WindowPoint b;
};

class SubjectView : public View {
  public:
    virtual void moveDotTo(WindowPoint) = 0;
};

class TesterView : public View {
  public:
    virtual void drawRed(Line) = 0;
    virtual void drawGreen(Line) = 0;
    virtual void drawWhiteCircleWithCenter(WindowPoint) = 0;
    virtual void clear() = 0;
};

class SubjectPresenterImpl : public SubjectPresenter, public Timer::Observer {
  public:
    SubjectPresenterImpl(
        SubjectView &, av_speech_in_noise::SubjectPresenter &, Timer &);
    void attach(SubjectPresenter::Observer *a) override;
    void start() override;
    void stop() override;
    void present(Point) override;
    void callback() override;

  private:
    SubjectView &view;
    av_speech_in_noise::SubjectPresenter &parentPresenter;
    Timer &timer;
    SubjectPresenter::Observer *observer{};
};

class TesterPresenterImpl : public TesterPresenter {
  public:
    explicit TesterPresenterImpl(TesterView &);
    void start() override;
    void stop() override;
    void present(const Results &) override;

  private:
    TesterView &view;
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
    Controller(Control &, Interactor &);
    void notifyThatWindowHasBeenTouched(WindowPoint) override;
    void notifyThatSubmitButtonHasBeenClicked() override;
    void notifyThatMenuHasBeenSelected() override;

  private:
    Interactor &interactor;
    Control &control;
};

namespace validation {
class TesterView : public View {
  public:
    virtual void setLeftEyeAccuracyDegrees(const std::string &) = 0;
    virtual void setLeftEyePrecisionDegrees(const std::string &) = 0;
    virtual void setRightEyeAccuracyDegrees(const std::string &) = 0;
    virtual void setRightEyePrecisionDegrees(const std::string &) = 0;
};

class TesterPresenterImpl : public TesterPresenter {
  public:
    explicit TesterPresenterImpl(TesterView &);
    void start() override;
    void stop() override;
    void present(const Result &) override;

  private:
    TesterView &view;
};

class Control {
  public:
    class Observer {
      public:
        AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(Observer);
        virtual void notifyThatMenuHasBeenSelected() = 0;
        virtual void notifyThatCloseButtonHasBeenClicked() = 0;
    };

    AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(Control);
    virtual void attach(Observer *) = 0;
};

class Controller : public Control::Observer {
  public:
    Controller(Control &, Interactor &);
    void notifyThatMenuHasBeenSelected() override;
    void notifyThatCloseButtonHasBeenClicked() override;

  private:
    Interactor &interactor;
};
}
}

#endif
