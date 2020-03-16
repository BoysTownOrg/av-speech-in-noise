#ifndef AV_SPEECH_IN_NOISE_RECOGNITION_TEST_INCLUDE_RECOGNITION_TEST_ADAPTIVEMETHOD_HPP_
#define AV_SPEECH_IN_NOISE_RECOGNITION_TEST_INCLUDE_RECOGNITION_TEST_ADAPTIVEMETHOD_HPP_

#include "Model.hpp"
#include "Randomizer.hpp"
#include <gsl/gsl>
#include <limits>
#include <memory>
#include <vector>
#include <string>

namespace av_speech_in_noise {
constexpr auto maximumInt() -> int { return std::numeric_limits<int>::max(); }

constexpr auto minimumInt() -> int { return std::numeric_limits<int>::min(); }

class Track {
  public:
    virtual ~Track() = default;
    struct Settings {
        const TrackingRule *rule{};
        int startingX{};
        int ceiling{maximumInt()};
        int floor{minimumInt()};
        int bumpLimit{maximumInt()};
    };
    virtual void down() = 0;
    virtual void up() = 0;
    virtual auto x() -> int = 0;
    virtual auto complete() -> bool = 0;
    virtual auto reversals() -> int = 0;
    virtual void reset() = 0;
    virtual auto threshold(int reversals) -> double = 0;

    class Factory {
      public:
        virtual ~Factory() = default;
        virtual auto make(const Settings &) -> std::shared_ptr<Track> = 0;
    };
};

struct TargetListWithTrack {
    std::shared_ptr<TargetList> list;
    std::shared_ptr<Track> track;
};

class AdaptiveMethodImpl : public AdaptiveMethod {
  public:
    AdaptiveMethodImpl(Track::Factory &, ResponseEvaluator &, Randomizer &);
    void initialize(const AdaptiveTest &, TargetListReader *) override;
    auto snr_dB() -> int override;
    void submitIncorrectResponse() override;
    void submitCorrectResponse() override;
    void submit(const open_set::CorrectKeywords &) override;
    auto complete() -> bool override;
    auto nextTarget() -> std::string override;
    auto currentTarget() -> std::string override;
    void writeLastCoordinateResponse(OutputFile *) override;
    void writeLastCorrectResponse(OutputFile *) override;
    void writeLastIncorrectResponse(OutputFile *) override;
    void writeLastCorrectKeywords(OutputFile *) override;
    void writeTestingParameters(OutputFile *) override;
    void writeTestResult(OutputFile *);
    void submit(const coordinate_response_measure::Response &) override;
    void submit(const open_set::FreeResponse &) override;
    void resetTracks() override;

  private:
    void selectNextList();
    void moveCompleteTracksToEnd();
    auto correct(const std::string &,
        const coordinate_response_measure::Response &) -> bool;
    void incorrect();
    void correct();

    std::vector<TargetListWithTrack> targetListsWithTracks{};
    coordinate_response_measure::AdaptiveTrial lastTrial{};
    open_set::AdaptiveTrial lastOpenSetTrial{};
    open_set::CorrectKeywordsTrial lastCorrectKeywordsTrial{};
    AdaptiveTestResult lastAdaptiveTestResult{};
    const AdaptiveTest *test{};
    Track::Factory &snrTrackFactory;
    ResponseEvaluator &evaluator;
    Randomizer &randomizer;
    Track *currentSnrTrack{};
    TargetList *currentTargetList{};
    gsl::index tracksInProgress{};
};
}

#endif
