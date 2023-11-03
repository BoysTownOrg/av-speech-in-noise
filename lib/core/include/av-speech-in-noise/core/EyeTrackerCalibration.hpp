#ifndef AV_SPEECH_IN_NOISE_LIB_CORE_INCLUDE_AVSPEECHINNOISE_CORE_EYETRACKERCALIBRATIONHPP_
#define AV_SPEECH_IN_NOISE_LIB_CORE_INCLUDE_AVSPEECHINNOISE_CORE_EYETRACKERCALIBRATIONHPP_

#include <av-speech-in-noise/Interface.hpp>

#include <utility>
#include <vector>
#include <ostream>

namespace av_speech_in_noise::eye_tracker_calibration {
struct Point {
    float x;
    float y;
};

struct Result {
    std::vector<Point> leftEyeMappedPoints;
    std::vector<Point> rightEyeMappedPoints;
    Point point{};
};

void write(std::ostream &, const std::vector<Result> &);

class Interactor {
  public:
    AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(Interactor);
    virtual void start() = 0;
    virtual void finish() = 0;
    virtual void redo(Point) = 0;
};

class SubjectPresenter {
  public:
    class Observer {
      public:
        AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(Observer);
        virtual void notifyThatPointIsReady() = 0;
    };
    AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(SubjectPresenter);
    virtual void attach(Observer *) = 0;
    virtual void start() = 0;
    virtual void stop() = 0;
    virtual void present(Point) = 0;
};

class TesterPresenter {
  public:
    AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(TesterPresenter);
    virtual void start() = 0;
    virtual void stop() = 0;
    virtual void present(const std::vector<Result> &) = 0;
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

class ResultsWriter {
  public:
    AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(ResultsWriter);
    virtual void write(const std::vector<Result> &) = 0;
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

class TesterPresenter {
  public:
    AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(TesterPresenter);
    virtual void present(const Result &) = 0;
    virtual void start() = 0;
    virtual void stop() = 0;
};

class Interactor {
  public:
    AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(Interactor);
    virtual void start() = 0;
    virtual void finish() = 0;
};

class InteractorImpl : public Interactor, public SubjectPresenter::Observer {
  public:
    explicit InteractorImpl(
        SubjectPresenter &, TesterPresenter &, Validator &, std::vector<Point>);
    void start() override;
    void finish() override;
    void notifyThatPointIsReady() override;

  private:
    std::vector<Point> points;
    std::vector<Point> pointsToValidate;
    SubjectPresenter &subjectPresenter;
    TesterPresenter &testerPresenter;
    Validator &validator;
};
}

class InteractorImpl : public SubjectPresenter::Observer, public Interactor {
  public:
    InteractorImpl(SubjectPresenter &, TesterPresenter &, Calibrator &,
        ResultsWriter &, std::vector<Point>);
    void start() override;
    void finish() override;
    void notifyThatPointIsReady() override;
    void redo(Point) override;

  private:
    std::vector<Point> points;
    std::vector<Point> pointsToCalibrate;
    SubjectPresenter &subjectPresenter;
    TesterPresenter &testerPresenter;
    Calibrator &calibrator;
    ResultsWriter &writer;
};
}

#endif
