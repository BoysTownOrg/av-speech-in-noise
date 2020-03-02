#include "TestSettingsInterpreter.hpp"
#include <gsl/gsl>
#include <sstream>

namespace av_speech_in_noise {
static auto entryDelimiter(const std::string &s) -> gsl::index {
    return s.find(':');
}

static auto nextLine(std::stringstream &stream) -> std::string {
    std::string line;
    std::getline(stream, line);
    return line;
}

void TestSettingsInterpreterImpl::apply(
    Model &model, const std::string &contents) {
    AdaptiveTest test;
    std::stringstream stream{contents};
    for (auto line{nextLine(stream)}; !line.empty(); line = nextLine(stream)) {
        auto entryName{line.substr(0, entryDelimiter(line))};
        auto entry{line.substr(entryDelimiter(line) + 2)};
        if (entryName == name(TestSetting::targets))
            test.targetListDirectory = entry;
        else if (entryName == name(TestSetting::masker))
            test.maskerFilePath = entry;
    }
    model.initializeTest(test);
}
}
