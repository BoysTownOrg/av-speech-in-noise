#include "SubmittingConsonant.hpp"

#include <regex>
#include <string>

namespace av_speech_in_noise::submitting_consonant {
InteractorImpl::InteractorImpl(
    FixedLevelMethod &method, RunningATest &model, OutputFile &outputFile)
    : method{method}, model{model}, outputFile{outputFile} {}

static auto consonant(const std::string &match) -> Consonant {
    if (match == "bi")
        return Consonant::bi;
    if (match == "si")
        return Consonant::si;
    if (match == "di")
        return Consonant::di;
    if (match == "hi")
        return Consonant::hi;
    if (match == "ki")
        return Consonant::ki;
    if (match == "mi")
        return Consonant::mi;
    if (match == "ni")
        return Consonant::ni;
    if (match == "pi")
        return Consonant::pi;
    if (match == "shi")
        return Consonant::shi;
    if (match == "ti")
        return Consonant::ti;
    if (match == "vi")
        return Consonant::vi;
    if (match == "zi")
        return Consonant::zi;
    if (match == "thi")
        return Consonant::thi;
    if (match == "fi")
        return Consonant::fi;
    return Consonant::unknown;
}

static auto correctConsonant(const LocalUrl &url) -> Consonant {
    const std::string stem{std::filesystem::path{url.path}.stem()};
    std::regex pattern{"choose_(.*?)_.*"};
    std::smatch match;
    std::regex_search(stem, match, pattern);
    return match.size() > 1 ? consonant(match[1]) : Consonant::unknown;
}

void InteractorImpl::submit(const ConsonantResponse &r) {
    ConsonantTrial trial;
    trial.subjectConsonant = r.consonant;
    trial.correctConsonant = correctConsonant(method.currentTarget());
    trial.target =
        std::filesystem::path{method.currentTarget().path}.filename();
    trial.correct = trial.correctConsonant == trial.subjectConsonant;
    outputFile.write(trial);
    outputFile.save();
    method.submit(Flaggable{false});
    model.prepareNextTrialIfNeeded();
}
}
