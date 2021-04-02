#include "../run.h"
#include "../AppKitView.h"
#include <tobii_research.h>
#include <tobii_research_streams.h>
#include <gsl/gsl>
#include <vector>

namespace av_speech_in_noise {
class TobiiEyeTracker : public EyeTracker {
  public:
    TobiiEyeTracker();
    ~TobiiEyeTracker() override;
    void allocateRecordingTimeSeconds(double s) override;
    void start() override;
    void stop() override;
    auto gazeSamples() -> BinocularGazeSamples override;
    auto currentSystemTime() -> EyeTrackerSystemTime override;

  private:
    static void gaze_data_callback(
        TobiiResearchGazeData *gaze_data, void *self);
    void gazeDataReceived(TobiiResearchGazeData *gaze_data);

    std::vector<TobiiResearchGazeData> gazeData{};
    TobiiResearchEyeTrackers *eyeTrackers{};
    std::size_t head{};
};

static auto eyeTracker(TobiiResearchEyeTrackers *eyeTrackers)
    -> TobiiResearchEyeTracker * {
    return eyeTrackers == nullptr || eyeTrackers->count == 0U
        ? nullptr
        : eyeTrackers->eyetrackers[0];
}

TobiiEyeTracker::TobiiEyeTracker() {
    tobii_research_find_all_eyetrackers(&eyeTrackers);
}

TobiiEyeTracker::~TobiiEyeTracker() {
    tobii_research_free_eyetrackers(eyeTrackers);
}

void TobiiEyeTracker::allocateRecordingTimeSeconds(double seconds) {
    float gaze_output_frequency_Hz{};
    tobii_research_get_gaze_output_frequency(
        eyeTracker(eyeTrackers), &gaze_output_frequency_Hz);
    gazeData.resize(std::ceil(gaze_output_frequency_Hz * seconds) + 1);
    head = 0;
}

void TobiiEyeTracker::start() {
    tobii_research_subscribe_to_gaze_data(
        eyeTracker(eyeTrackers), gaze_data_callback, this);
}

void TobiiEyeTracker::stop() {
    tobii_research_unsubscribe_from_gaze_data(
        eyeTracker(eyeTrackers), gaze_data_callback);
}

void TobiiEyeTracker::gaze_data_callback(
    TobiiResearchGazeData *gaze_data, void *self) {
    static_cast<TobiiEyeTracker *>(self)->gazeDataReceived(gaze_data);
}

void TobiiEyeTracker::gazeDataReceived(TobiiResearchGazeData *gaze_data) {
    if (head < gazeData.size())
        gazeData.at(head++) = *gaze_data;
}

static auto at(std::vector<BinocularGazeSample> &b, gsl::index i)
    -> BinocularGazeSample & {
    return b.at(i);
}

static auto at(const std::vector<TobiiResearchGazeData> &b, gsl::index i)
    -> const TobiiResearchGazeData & {
    return b.at(i);
}

static auto eyeGaze(const TobiiResearchEyeData &d)
    -> const TobiiResearchNormalizedPoint2D & {
    return d.gaze_point.position_on_display_area;
}

static auto leftEyeGaze(const std::vector<TobiiResearchGazeData> &gaze,
    gsl::index i) -> const TobiiResearchNormalizedPoint2D & {
    return eyeGaze(at(gaze, i).left_eye);
}

static auto rightEyeGaze(const std::vector<TobiiResearchGazeData> &gaze,
    gsl::index i) -> const TobiiResearchNormalizedPoint2D & {
    return eyeGaze(at(gaze, i).right_eye);
}

static auto x(const TobiiResearchNormalizedPoint2D &p) -> float { return p.x; }

static auto y(const TobiiResearchNormalizedPoint2D &p) -> float { return p.y; }

static auto leftEyeGaze(std::vector<BinocularGazeSample> &b, gsl::index i)
    -> EyeGaze & {
    return at(b, i).left;
}

static auto rightEyeGaze(BinocularGazeSamples &b, gsl::index i) -> EyeGaze & {
    return at(b, i).right;
}

static auto x(EyeGaze &p) -> float & { return p.x; }

static auto y(EyeGaze &p) -> float & { return p.y; }

static auto size(const std::vector<BinocularGazeSample> &v) -> gsl::index {
    return v.size();
}

auto TobiiEyeTracker::gazeSamples() -> BinocularGazeSamples {
    BinocularGazeSamples gazeSamples_(head > 0 ? head - 1 : 0);
    for (gsl::index i{0}; i < size(gazeSamples_); ++i) {
        at(gazeSamples_, i).systemTime.microseconds =
            at(gazeData, i).system_time_stamp;
        x(leftEyeGaze(gazeSamples_, i)) = x(leftEyeGaze(gazeData, i));
        y(leftEyeGaze(gazeSamples_, i)) = y(leftEyeGaze(gazeData, i));
        x(rightEyeGaze(gazeSamples_, i)) = x(rightEyeGaze(gazeData, i));
        y(rightEyeGaze(gazeSamples_, i)) = y(rightEyeGaze(gazeData, i));
    }
    return gazeSamples_;
}

auto TobiiEyeTracker::currentSystemTime() -> EyeTrackerSystemTime {
    EyeTrackerSystemTime currentSystemTime{};
    int64_t microseconds = 0;
    tobii_research_get_system_time_stamp(&microseconds);
    currentSystemTime.microseconds = microseconds;
    return currentSystemTime;
}
}

int main() {
    av_speech_in_noise::TobiiEyeTracker eyeTracker;
    av_speech_in_noise::AppKitTestSetupUIFactoryImpl testSetupViewFactory;
    av_speech_in_noise::DefaultOutputFileNameFactory outputFileNameFactory;
    av_speech_in_noise::initializeAppAndRunEventLoop(
        eyeTracker, testSetupViewFactory, outputFileNameFactory);
}
