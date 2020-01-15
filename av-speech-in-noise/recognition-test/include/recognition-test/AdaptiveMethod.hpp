#ifndef AV_SPEECH_IN_NOISE_RECOGNITION_TEST_INCLUDE_RECOGNITION_TEST_ADAPTIVEMETHOD_HPP_
#define AV_SPEECH_IN_NOISE_RECOGNITION_TEST_INCLUDE_RECOGNITION_TEST_ADAPTIVEMETHOD_HPP_

#include "Model.hpp"
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

    class Factory {
      public:
        virtual ~Factory() = default;
        virtual auto make(const Settings &) -> std::shared_ptr<Track> = 0;
    };
};

class TrackSettingsReader {
  public:
    virtual ~TrackSettingsReader() = default;
    virtual auto read(std::string) -> const TrackingRule * = 0;
};

class TargetListReader {
  public:
    virtual ~TargetListReader() = default;
    using lists_type = typename std::vector<std::shared_ptr<TargetList>>;
    virtual auto read(std::string directory) -> lists_type = 0;
};

class AdaptiveMethodImpl : public AdaptiveMethod {
  public:
    AdaptiveMethodImpl(TargetListReader *, TrackSettingsReader *,
        Track::Factory *, ResponseEvaluator *, Randomizer *);
    void initialize(const AdaptiveTest &) override;
    auto snr_dB() -> int override;
    void submitIncorrectResponse() override;
    void submitCorrectResponse() override;
    auto complete() -> bool override;
    auto next() -> std::string override;
    auto current() -> std::string override;
    void writeLastCoordinateResponse(OutputFile *) override;
    void writeLastCorrectResponse(OutputFile *) override;
    void writeLastIncorrectResponse(OutputFile *) override;
    void writeTestingParameters(OutputFile *) override;
    void submitResponse(const coordinate_response_measure::Response &) override;
    void submitResponse(const open_set::FreeResponse &) override;

  private:
    struct TargetListWithTrack {
        TargetList *list;
        std::shared_ptr<Track> track;
    };
    void selectNextListAfter(void (AdaptiveMethodImpl::*)());
    void prepareSnrTracks();
    void makeSnrTracks();
    void makeTrackWithList(TargetList *list);
    void selectNextList();
    void removeCompleteTracks();
    static auto complete(const TargetListWithTrack &) -> bool;
    static auto track(const TargetListWithTrack &) -> Track *;
    auto correct(const std::string &,
        const coordinate_response_measure::Response &) -> bool;
    void incorrect();
    void correct();

    TargetListReader::lists_type lists{};
    std::vector<TargetListWithTrack> targetListsWithTracks{};
    Track::Settings trackSettings{};
    coordinate_response_measure::AdaptiveTrial lastTrial{};
    open_set::AdaptiveTrial lastOpenSetTrial{};
    const AdaptiveTest *test{};
    TargetListReader *targetListSetReader;
    TrackSettingsReader *trackSettingsReader;
    Track::Factory *snrTrackFactory;
    ResponseEvaluator *evaluator;
    Randomizer *randomizer;
    Track *currentSnrTrack{};
    TargetList *currentTargetList{};
};
}

#endif
