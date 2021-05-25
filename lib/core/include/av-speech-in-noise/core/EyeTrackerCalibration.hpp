#ifndef AV_SPEECH_IN_NOISE_LIB_CORE_INCLUDE_AVSPEECHINNOISE_CORE_EYETRACKERCALIBRATIONHPP_
#define AV_SPEECH_IN_NOISE_LIB_CORE_INCLUDE_AVSPEECHINNOISE_CORE_EYETRACKERCALIBRATIONHPP_

#include <av-speech-in-noise/Interface.hpp>
#include <utility>
#include <vector>

namespace av_speech_in_noise::eye_tracking::calibration {
struct Point {
    float x;
    float y;
};

struct Result {
    std::vector<Point> leftEyeMappedPoints;
    std::vector<Point> rightEyeMappedPoints;
    Point point{};
};

class IInteractor {
  public:
    AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(IInteractor);
    virtual void redo(Point) = 0;
    virtual void finish() = 0;
    virtual void start() = 0;
};

namespace validation {
struct Result;
}

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
    virtual void present(const std::vector<Result> &) = 0;
    virtual void present(const validation::Result &) = 0;
    virtual void stop() = 0;
    virtual void start() = 0;
};

class Calibrator {
  public:
    AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(Calibrator);
    virtual void acquire() = 0;
    virtual void release() = 0;
    virtual void collect(Point) = 0;
    virtual void discard(Point) = 0;
    virtual auto results() -> std::vector<Result> = 0;
};

namespace validation {
struct Angle {
    float degrees;
};

struct MonocularResult {
    Angle errorOfMeanGaze;
    Angle standardDeviationFromTheMeanGaze;
};

struct Result {
    MonocularResult left;
    MonocularResult right;
};

class Validator {
  public:
    AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(Validator);
    virtual void acquire() = 0;
    virtual void release() = 0;
    virtual void collect(Point) = 0;
    virtual auto result() -> Result = 0;
};

class Interactor : IPresenter::Observer {
  public:
    explicit Interactor(IPresenter &, Validator &, std::vector<Point>);
    void start();
    void finish();
    void notifyThatPointIsReady() override;

  private:
    std::vector<Point> points;
    std::vector<Point> pointsToValidate;
    IPresenter &presenter;
    Validator &validator;
};
}

class Interactor : public IPresenter::Observer, public IInteractor {
  public:
    Interactor(IPresenter &, Calibrator &, std::vector<Point>);
    void notifyThatPointIsReady() override;
    void start() override;
    void finish() override;
    void redo(Point) override;

  private:
    std::vector<Point> points;
    std::vector<Point> pointsToCalibrate;
    IPresenter &presenter;
    Calibrator &calibrator;
};
}

#endif
