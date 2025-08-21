#ifndef AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_TESTSETTINGSINTERPRETER_HPP_
#define AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_TESTSETTINGSINTERPRETER_HPP_

#include "TestSetupImpl.hpp"

#include <av-speech-in-noise/core/Configuration.hpp>
#include <av-speech-in-noise/core/IAdaptiveMethod.hpp>
#include <av-speech-in-noise/core/IFixedLevelMethod.hpp>
#include <av-speech-in-noise/core/IRunningATest.hpp>
#include <av-speech-in-noise/core/TargetPlaylist.hpp>

#include <cstdint>
#include <functional>
#include <map>
#include <string>
#include <vector>

namespace av_speech_in_noise {
enum class TestSetting : std::uint8_t {
    method,
    masker,
    maskerLevel,
};

constexpr auto name(TestSetting p) -> const char * {
    switch (p) {
    case TestSetting::method:
        return "method";
    case TestSetting::masker:
        return "masker";
    case TestSetting::maskerLevel:
        return "masker level (dB SPL)";
    }
}

class TestSettingsInterpreterImpl : public TestSettingsInterpreter,
                                    public ConfigurationRegistry {
  public:
    TestSettingsInterpreterImpl(
        RunningATest &runningATest, SessionController &sessionController);
    void initializeTest(const std::string &, const TestIdentity &,
        const std::string &startingSNR) override;
    static auto meta(const std::string &) -> std::string;
    auto calibration(const std::string &) -> Calibration override;
    void subscribe(Configurable &c, const std::string &key) override {
        configurables[key].emplace_back(c);
    }

  private:
    std::map<std::string, std::vector<std::reference_wrapper<Configurable>>>
        configurables;
    RunningATest &runningATest;
    SessionController &sessionController;
};
}

#endif
