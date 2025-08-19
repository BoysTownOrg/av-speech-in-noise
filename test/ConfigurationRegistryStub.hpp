#ifndef AV_SPEECH_IN_NOISE_TEST_CONFIGURATIONREGISTRY_HPP_
#define AV_SPEECH_IN_NOISE_TEST_CONFIGURATIONREGISTRY_HPP_

#include <av-speech-in-noise/core/Configuration.hpp>

namespace av_speech_in_noise {
class ConfigurationRegistryStub : public ConfigurationRegistry {
  public:
    void subscribe(Configurable &, const std::string &) override {}
};
}

#endif
