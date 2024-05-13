#include "KeyPress.hpp"

namespace av_speech_in_noise::submitting_keypress {
Presenter::Presenter(TestView &testView, TestController &testController,
    Interactor &interactor, Control &control)
    : testView{testView}, testController{testController},
      interactor{interactor}, control{control} {
    control.attach(this);
}

/*
static auto nanoseconds(Delay x) -> std::uintmax_t {
    return gsl::narrow_cast<std::uintmax_t>(x.seconds * 1e9);
}

static auto nanoseconds(MaskerPlayer &player, const PlayerTime &t)
    -> std::uintmax_t {
    return player.nanoseconds(t);
}

static auto nanoseconds(MaskerPlayer &player, const PlayerTimeWithDelay &t)
    -> std::uintmax_t {
    return nanoseconds(player, t.playerTime) + nanoseconds(t.delay);
}
*/

void Presenter::notifyThatTargetWillPlayAt(const PlayerTimeWithDelay &t) {
    // targetStartTimeMilliseconds =
    // static_cast<double>((nanoseconds(maskerPlayer, t) + 500_000) /
    // 1000_000);
}

void Presenter::notifyThatKeyHasBeenPressed() {
    if (ready) {
        auto response{KeyPressResponse{}};
        // auto seconds{control.keyPressedSeconds()};
        // response.reactionTimeMilliseconds = seconds * 1000 -
        // targetStartTimeMilliseconds;
        const auto keyPressed{control.keyPressed()};
        if (keyPressed == "1")
            response.key = KeyPressed::first;
        else if (keyPressed == "2")
            response.key = KeyPressed::second;
        else
            response.key = KeyPressed::unknown;
        interactor.submit(response);
        testController.notifyThatUserIsDoneResponding();
    }
}

void Presenter::start() { testView.showNextTrialButton(); }

void Presenter::stop() { ready = false; }

void Presenter::hideResponseSubmission() { ready = false; }

void Presenter::showResponseSubmission() { ready = true; }
}
