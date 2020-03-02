#ifndef AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_TESTSETTINGSINTERPRETER_HPP_
#define AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_TESTSETTINGSINTERPRETER_HPP_

#include "Presenter.hpp"

namespace av_speech_in_noise {
enum class TestSetting { method };

constexpr auto name(TestSetting p) -> const char * {
    switch (p) {
    case TestSetting::method:
        return "method";
    }
}

class TestSettingsInterpreterImpl : public TestSettingsInterpreter {
  public:
    void apply(Model &, const std::string &) override;
};
}

#endif
