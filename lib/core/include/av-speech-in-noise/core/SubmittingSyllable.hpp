#ifndef AV_SPEECH_IN_NOISE_LIB_CORE_INCLUDE_AVSPEECHINNOISE_CORE_SUBMITTINGSYLLABLEHPP_
#define AV_SPEECH_IN_NOISE_LIB_CORE_INCLUDE_AVSPEECHINNOISE_CORE_SUBMITTINGSYLLABLEHPP_

#include "IModel.hpp"
#include "IFixedLevelMethod.hpp"
#include "IOutputFile.hpp"
#include "IRecognitionTestModel.hpp"
#include <filesystem>
#include <regex>

namespace av_speech_in_noise::submitting_syllable {
static auto syllable(const std::string &match) -> Syllable {
    if (match == "bi")
        return Syllable::bi;
    if (match == "di")
        return Syllable::di;
    if (match == "dji")
        return Syllable::dji;
    if (match == "fi")
        return Syllable::fi;
    if (match == "gi")
        return Syllable::gi;
    if (match == "hi")
        return Syllable::hi;
    if (match == "ji")
        return Syllable::ji;
    if (match == "ki")
        return Syllable::ki;
    if (match == "li")
        return Syllable::li;
    if (match == "mi")
        return Syllable::mi;
    if (match == "ni")
        return Syllable::ni;
    if (match == "pi")
        return Syllable::pi;
    if (match == "ri")
        return Syllable::ri;
    if (match == "shi")
        return Syllable::shi;
    if (match == "si")
        return Syllable::si;
    if (match == "thi")
        return Syllable::thi;
    if (match == "ti")
        return Syllable::ti;
    if (match == "tsi")
        return Syllable::tsi;
    if (match == "vi")
        return Syllable::vi;
    if (match == "wi")
        return Syllable::wi;
    if (match == "zi")
        return Syllable::zi;
    return Syllable::unknown;
}

static auto correctSyllable(const LocalUrl &file) -> Syllable {
    const auto stem{std::filesystem::path{file.path}.stem().string()};
    std::regex pattern{"say_(.*?)_.*"};
    std::smatch match;
    std::regex_search(stem, match, pattern);
    return match.size() > 1 ? syllable(match[1]) : Syllable::unknown;
}

class InteractorImpl : public Interactor {
  public:
    InteractorImpl(FixedLevelMethod &method, RecognitionTestModel &model,
        OutputFile &outputFile)
        : method{method}, model{model}, outputFile{outputFile} {}

    void submit(const SyllableResponse &p) override {
        method.submit(p);
        SyllableTrial trial;
        trial.subjectSyllable = p.syllable;
        trial.target =
            std::filesystem::path{method.currentTarget().path}.filename();
        trial.correctSyllable = correctSyllable(method.currentTarget());
        trial.correct = trial.correctSyllable == p.syllable;
        trial.flagged = p.flagged;
        outputFile.write(trial);
        outputFile.save();
        model.prepareNextTrialIfNeeded();
    }

  private:
    FixedLevelMethod &method;
    RecognitionTestModel &model;
    OutputFile &outputFile;
};
}

#endif
