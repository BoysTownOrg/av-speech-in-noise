#ifndef AV_SPEECH_IN_NOISE_LIB_CORE_INCLUDE_AVSPEECHINNOISE_CORE_ADAPTIVETRACKFACTORYHPP_
#define AV_SPEECH_IN_NOISE_LIB_CORE_INCLUDE_AVSPEECHINNOISE_CORE_ADAPTIVETRACKFACTORYHPP_

#include "Configuration.hpp"
#include "IAdaptiveMethod.hpp"
#include "LevittTrack.hpp"
#include "UpdatedMaximumLikelihood.hpp"

namespace av_speech_in_noise {
class AdaptiveTrackFactory : public AdaptiveTrack::Factory,
                             public Configurable {
  public:
    explicit AdaptiveTrackFactory(ConfigurationRegistry &);
    auto make(const AdaptiveTrack::Settings &s)
        -> std::shared_ptr<AdaptiveTrack> override;
    void configure(const std::string &key, const std::string &value) override;
    void write(std::ostream &) override;

    UmlSettings umlSettings;
    LevittSettings levittSettings;
    bool uml{};
};
}

#endif
