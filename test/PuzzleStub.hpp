#ifndef AV_SPEECH_IN_NOISE_TESTS_PUZZLESTUB_HPP_
#define AV_SPEECH_IN_NOISE_TESTS_PUZZLESTUB_HPP_

#include "av-speech-in-noise/Model.hpp"
#include <av-speech-in-noise/ui/FreeResponse.hpp>

namespace av_speech_in_noise::submitting_free_response::with_puzzle {
class PuzzleStub : public Puzzle {
  public:
    [[nodiscard]] auto advanced() const -> bool { return advanced_; }

    void advance() override { advanced_ = true; }

    void reset() override { hasBeenReset_ = true; }

    [[nodiscard]] auto hasBeenReset() const -> bool { return hasBeenReset_; }

    [[nodiscard]] auto shown() const -> bool { return shown_; }

    void show() override { shown_ = true; }

    [[nodiscard]] auto hidden() const -> bool { return hidden_; }

    void hide() override { hidden_ = true; }

    void clearAdvanced() { advanced_ = false; }

    auto url() -> LocalUrl { return url_; }

    void initialize(const LocalUrl &p) { url_ = p; }

  private:
    LocalUrl url_;
    bool advanced_{};
    bool shown_{};
    bool hidden_{};
    bool hasBeenReset_{};
};
}

#endif
