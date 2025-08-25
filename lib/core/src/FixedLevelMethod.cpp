#include "FixedLevelMethod.hpp"
#include "Configuration.hpp"

#include <sstream>
#include <stdexcept>

namespace av_speech_in_noise {
FixedLevelMethodImpl::FixedLevelMethodImpl(ConfigurationRegistry &registry,
    ResponseEvaluator &evaluator, RunningATest &runningATest)
    : evaluator{evaluator}, runningATest{runningATest} {
    registry.subscribe(*this, "targets");
    registry.subscribe(*this, "starting SNR (dB)");
    registry.subscribe(*this, "method");
}

void FixedLevelMethodImpl::configure(
    const std::string &key, const std::string &value) {
    if (key == "targets")
        targetsUrl.path = value;
    else if (key == "starting SNR (dB)")
        snr_.dB = integer(value);
    else if (key == "method")
        if (contains(value, "fixed-level"))
            runningATest.attach(this);
}

static void load(TargetPlaylist *list, const LocalUrl &targetsUrl) {
    list->load(targetsUrl);
}

static void initialize(TargetPlaylist *targetList, const LocalUrl &targetsUrl) {
    try {
        load(targetList, targetsUrl);
    } catch (const FiniteTargetPlaylist::LoadFailure &) {
        std::stringstream stream;
        stream << "Unable to load targets from " << targetsUrl.path;
        throw std::runtime_error{stream.str()};
    }
}

void FixedLevelMethodImpl::initialize() {
    av_speech_in_noise::initialize(targetList, targetsUrl);
    if (usingFiniteTargetPlaylist_)
        finiteTargetsExhausted_ = finiteTargetPlaylist->empty();
}

void FixedLevelMethodImpl::attach(
    const FixedLevelFixedTrialsTest &test, TargetPlaylist *list) {
    targetList = list;
    usingFiniteTargetPlaylist_ = false;
    trials_ = test.trials;
}

void FixedLevelMethodImpl::attach(FiniteTargetPlaylistWithRepeatables *list) {
    targetList = list;
    usingFiniteTargetPlaylist_ = true;
    finiteTargetPlaylist = list;
    finiteTargetPlaylistWithRepeatables = list;
    totalKeywordsSubmitted_ = 0;
    totalKeywordsCorrect_ = 0;
}

void FixedLevelMethodImpl::attach(FiniteTargetPlaylist *list) {
    targetList = list;
    usingFiniteTargetPlaylist_ = true;
    finiteTargetPlaylist = list;
}

auto FixedLevelMethodImpl::complete() -> bool {
    return usingFiniteTargetPlaylist_ ? finiteTargetsExhausted_ : trials_ == 0;
}

auto FixedLevelMethodImpl::nextTarget() -> LocalUrl {
    return targetList->next();
}

auto FixedLevelMethodImpl::snr() -> FloatSNR {
    FloatSNR snr{};
    snr.dB = snr_.dB;
    return snr;
}

static auto current(TargetPlaylist *list) -> LocalUrl {
    return list->current();
}

void FixedLevelMethodImpl::write(std::ostream &stream) {
    insertLabeledLine(stream, "targets", targetsUrl.path);
    insertLabeledLine(stream, "SNR (dB)", snr_.dB);
}

void FixedLevelMethodImpl::submit(
    const coordinate_response_measure::Response &response) {
    lastCoordinateResponseMeasureTrial.subjectColor = response.color;
    lastCoordinateResponseMeasureTrial.subjectNumber = response.number;
    lastCoordinateResponseMeasureTrial.correctColor =
        evaluator.correctColor(current(targetList));
    lastCoordinateResponseMeasureTrial.correctNumber =
        evaluator.correctNumber(current(targetList));
    lastCoordinateResponseMeasureTrial.correct =
        evaluator.correct(current(targetList), response);
    lastCoordinateResponseMeasureTrial.target = current(targetList).path;
    if (usingFiniteTargetPlaylist_)
        finiteTargetsExhausted_ = finiteTargetPlaylist->empty();
    else
        --trials_;
}

auto FixedLevelMethodImpl::currentTarget() -> LocalUrl {
    return current(targetList);
}

void FixedLevelMethodImpl::writeLastCoordinateResponse(OutputFile &file) {
    file.write(lastCoordinateResponseMeasureTrial);
}

static void update(
    FiniteTargetPlaylistWithRepeatables *finiteTargetPlaylistWithRepeatables,
    FiniteTargetPlaylist *finiteTargetPlaylist, bool flagged,
    bool &finiteTargetsExhausted_) {
    if (flagged)
        finiteTargetPlaylistWithRepeatables->reinsertCurrent();
    finiteTargetsExhausted_ = finiteTargetPlaylist->empty();
}

void FixedLevelMethodImpl::submit(const Flaggable &response) {
    if (usingFiniteTargetPlaylist_)
        update(finiteTargetPlaylistWithRepeatables, finiteTargetPlaylist,
            response.flagged, finiteTargetsExhausted_);
    else
        --trials_;
}

void FixedLevelMethodImpl::submit(const ThreeKeywordsResponse &response) {
    update(finiteTargetPlaylistWithRepeatables, finiteTargetPlaylist,
        response.flagged, finiteTargetsExhausted_);
    totalKeywordsSubmitted_ += 3;
    if (response.firstCorrect)
        ++totalKeywordsCorrect_;
    if (response.secondCorrect)
        ++totalKeywordsCorrect_;
    if (response.thirdCorrect)
        ++totalKeywordsCorrect_;
}

auto FixedLevelMethodImpl::keywordsTestResults() -> KeywordsTestResults {
    return KeywordsTestResults{totalKeywordsSubmitted_ == 0
            ? 0
            : totalKeywordsCorrect_ * 100. / totalKeywordsSubmitted_,
        totalKeywordsCorrect_};
}
}
