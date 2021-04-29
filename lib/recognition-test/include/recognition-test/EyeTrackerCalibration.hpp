#ifndef AV_SPEECH_IN_NOISE_RECOGNITION_TEST_INCLUDE_RECOGNITION_TEST_EYE_TRACKER_CALIBRATION_HPP_
#define AV_SPEECH_IN_NOISE_RECOGNITION_TEST_INCLUDE_RECOGNITION_TEST_EYE_TRACKER_CALIBRATION_HPP_

#include <av-speech-in-noise/Interface.hpp>
#include <utility>
#include <vector>

namespace av_speech_in_noise {
struct Point {
    float x;
    float y;
};

namespace eye_tracker_calibration {
class IPresenter {
  public:
    class Observer {
      public:
        AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(Observer);
        virtual void notifyThatPointIsReady() = 0;
    };
    AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(IPresenter);
    virtual void attach(Observer *) = 0;
    virtual void present(Point) = 0;
};

class EyeTrackerCalibrator {
  public:
    AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(EyeTrackerCalibrator);
    virtual void calibrate(Point) = 0;
};

class Interactor : public IPresenter::Observer {
  public:
    explicit Interactor(IPresenter &presenter, EyeTrackerCalibrator &calibrator,
        std::vector<Point> points)
        : points{std::move(points)}, presenter{presenter}, calibrator{
                                                               calibrator} {
        presenter.attach(this);
    }

    void notifyThatPointIsReady() override {
        calibrator.calibrate(points.front());
    }

    void calibrate() { presenter.present(points.front()); }

  private:
    std::vector<Point> points;
    IPresenter &presenter;
    EyeTrackerCalibrator &calibrator;
};
}
}

#endif
