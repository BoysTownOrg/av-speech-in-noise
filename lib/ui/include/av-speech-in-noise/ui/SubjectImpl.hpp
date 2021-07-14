#ifndef AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_SUBJECTIMPL_HPP_
#define AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_SUBJECTIMPL_HPP_

#include "Subject.hpp"
#include "Session.hpp"

#include <algorithm>
#include <iterator>

namespace av_speech_in_noise {
static auto operator==(const Screen &a, const Screen &b) -> bool {
    return a.name == b.name;
}

class SubjectPresenterImpl : public SubjectPresenter {
  public:
    SubjectPresenterImpl(SubjectView &view, SessionView &sessionView)
        : view{view}, sessionView{sessionView} {}

    void start() override {
        const auto screens{sessionView.screens()};
        if (const auto it{std::find(
                screens.begin(), screens.end(), sessionView.subjectScreen())};
            it != screens.end())
            view.moveToScreen(std::distance(screens.begin(), it));
        view.show();
    }

    void stop() override { view.hide(); }

  private:
    SubjectView &view;
    SessionView &sessionView;
};
}

#endif
