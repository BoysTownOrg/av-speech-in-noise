#ifndef AV_SPEECH_IN_NOISE_LIB_CORE_INCLUDE_AVSPEECHINNOISE_CORE_TARGETPLAYLISTHPP_
#define AV_SPEECH_IN_NOISE_LIB_CORE_INCLUDE_AVSPEECHINNOISE_CORE_TARGETPLAYLISTHPP_

#include <av-speech-in-noise/Model.hpp>
#include <gsl/gsl>
#include <string>

namespace av_speech_in_noise {
class TargetPlaylist {
  public:
    virtual ~TargetPlaylist() = default;
    virtual void loadFromDirectory(const LocalUrl &directory) = 0;
    virtual auto next() -> LocalUrl = 0;
    virtual auto current() -> LocalUrl = 0;
    virtual auto directory() -> LocalUrl = 0;
};

class FiniteTargetPlaylist : public virtual TargetPlaylist {
  public:
    virtual auto empty() -> bool = 0;
};

class RepeatableFiniteTargetPlaylist : public virtual FiniteTargetPlaylist {
  public:
    virtual void setRepeats(gsl::index) = 0;
};

class FiniteTargetPlaylistWithRepeatables
    : public virtual FiniteTargetPlaylist {
  public:
    virtual void reinsertCurrent() = 0;
};
}

#endif
