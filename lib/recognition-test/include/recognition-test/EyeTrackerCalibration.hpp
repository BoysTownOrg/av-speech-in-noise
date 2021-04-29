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

static auto consumeOne(std::vector<Point> &x) -> Point {
    auto point = x.front();
    x.erase(x.begin());
    return point;
}

static void present(IPresenter &presenter, const std::vector<Point> &points) {
    if (!points.empty())
        presenter.present(points.front());
}

class Interactor : public IPresenter::Observer {
  public:
    explicit Interactor(IPresenter &presenter, EyeTrackerCalibrator &calibrator,
        std::vector<Point> points)
        : points{std::move(points)}, presenter{presenter}, calibrator{
                                                               calibrator} {
        presenter.attach(this);
    }

    void notifyThatPointIsReady() override {
        calibrator.calibrate(consumeOne(points));
        present(presenter, points);
    }

    void calibrate() { present(presenter, points); }

  private:
    std::vector<Point> points;
    IPresenter &presenter;
    EyeTrackerCalibrator &calibrator;
};
}
}

#endif
