#include "assert-utility.hpp"
#include "ModelStub.hpp"
#include "SessionViewStub.hpp"

#include <av-speech-in-noise/ui/SubjectImpl.hpp>

#include <gtest/gtest.h>

#include <utility>

namespace av_speech_in_noise {
namespace {
class SubjectViewStub : public SubjectView {
  public:
    [[nodiscard]] auto screenIndex() const -> int { return screenIndex_; }

    void moveToScreen(int index) override { screenIndex_ = index; }

  private:
    int screenIndex_{};
};

class SubjectPresenterTests : public ::testing::Test {
  protected:
    SubjectViewStub view;
    SessionViewStub sessionView;
    SubjectPresenterImpl presenter{view, sessionView};
};

#define SUBJECT_PRESENTER_TEST(a) TEST_F(SubjectPresenterTests, a)

SUBJECT_PRESENTER_TEST(startMovesSubjectViewToScreen) {
    sessionView.setScreens({{"a"}, {"b"}, {"c"}, {"d"}});
    sessionView.setSubject(Screen{"c"});
    presenter.start();
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(2, view.screenIndex());
}
}
}
