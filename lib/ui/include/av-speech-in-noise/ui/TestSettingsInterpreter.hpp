#ifndef AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_TESTSETTINGSINTERPRETER_HPP_
#define AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_TESTSETTINGSINTERPRETER_HPP_

#include "TestSetupImpl.hpp"

#include <av-speech-in-noise/core/Configuration.hpp>
#include <av-speech-in-noise/core/IRunningATest.hpp>

#include <functional>
#include <map>
#include <string>
#include <vector>

namespace av_speech_in_noise {
class TestSettingsInterpreterImpl : public TestSettingsInterpreter,
                                    public ConfigurationRegistry {
  public:
    TestSettingsInterpreterImpl(
        RunningATest &runningATest, SessionController &sessionController);
    void initializeTest(const std::string &, const TestIdentity &,
        const std::string &startingSNR) override;
    void apply(
        const std::string &, const std::vector<std::string> &matches) override;
    static auto meta(const std::string &) -> std::string;
    void subscribe(Configurable &c, const std::string &key) override;

  private:
    std::map<std::string, std::vector<std::reference_wrapper<Configurable>>>
        configurables;
    RunningATest &runningATest;
    SessionController &sessionController;
};
}

#endif
