#ifndef AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_TESTSETUP_HPP_
#define AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_TESTSETUP_HPP_

#include "View.hpp"
#include "Presenter.hpp"
#include <av-speech-in-noise/Interface.hpp>
#include <vector>
#include <string>
#include <string_view>

namespace av_speech_in_noise {
class TestSetupControl {
  public:
    class Observer {
      public:
        AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(Observer);
        virtual void notifyThatConfirmButtonHasBeenClicked() = 0;
        virtual void notifyThatPlayCalibrationButtonHasBeenClicked() = 0;
        virtual void
        notifyThatPlayLeftSpeakerCalibrationButtonHasBeenClicked() = 0;
        virtual void
        notifyThatPlayRightSpeakerCalibrationButtonHasBeenClicked() = 0;
    };
    AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(TestSetupControl);
    virtual void attach(Observer *) = 0;
    virtual auto testSettingsFile() -> std::string = 0;
    virtual auto startingSnr() -> std::string = 0;
    virtual auto testerId() -> std::string = 0;
    virtual auto subjectId() -> std::string = 0;
    virtual auto session() -> std::string = 0;
    virtual auto rmeSetting() -> std::string = 0;
    virtual auto transducer() -> std::string = 0;
};

class TestSetupView : public virtual View {
  public:
    virtual void populateTransducerMenu(std::vector<std::string>) = 0;
};

class TestSetupPresenter : public Presenter {
  public:
    virtual void updateErrorMessage(std::string_view) = 0;
};
}

#endif
