#include "TestSettingsInterpreter.hpp"
#include <sstream>

namespace av_speech_in_noise {
void TestSettingsInterpreterImpl::apply(
    Model &model, const std::string &contents) {
    AdaptiveTest test;
    std::stringstream stream{contents};
    std::string line;
    std::getline(stream, line);
    std::getline(stream, line);
    auto entryName{line.substr(0, line.find(':'))};
    if (entryName == name(TestSetting::targets)) {
        auto entry{line.substr(line.find(':') + 2)};
        test.targetListDirectory = entry;
    }
    model.initializeTest(test);
}
}
