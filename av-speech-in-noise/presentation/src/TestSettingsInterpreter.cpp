#include "TestSettingsInterpreter.hpp"

namespace av_speech_in_noise {
void TestSettingsInterpreterImpl::apply(Model &model, const std::string &) {
    model.initializeTest(AdaptiveTest{});
}
}
