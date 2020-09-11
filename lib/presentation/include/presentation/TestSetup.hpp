#ifndef AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_TESTSETUP_HPP_
#define AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_TESTSETUP_HPP_

#include "PresenterSimple.hpp"
#include "Method.hpp"
#include <vector>
#include <string>

namespace av_speech_in_noise {
class TestSetupInputView {
  public:
    class EventListener {
      public:
        virtual ~EventListener() = default;
        virtual void notifyThatConfirmButtonHasBeenClicked() = 0;
        virtual void notifyThatPlayCalibrationButtonHasBeenClicked() = 0;
        virtual void notifyThatBrowseForTestSettingsButtonHasBeenClicked() = 0;
    };
    virtual void subscribe(EventListener *) = 0;
    virtual ~TestSetupInputView() = default;
    virtual auto testSettingsFile() -> std::string = 0;
    virtual auto startingSnr() -> std::string = 0;
    virtual auto testerId() -> std::string = 0;
    virtual auto subjectId() -> std::string = 0;
    virtual auto session() -> std::string = 0;
    virtual auto rmeSetting() -> std::string = 0;
    virtual auto transducer() -> std::string = 0;
};

class TestSetupOutputView {
  public:
    virtual ~TestSetupOutputView() = default;
    virtual void populateTransducerMenu(std::vector<std::string>) = 0;
    virtual void setTestSettingsFile(std::string) = 0;
    virtual void show() = 0;
    virtual void hide() = 0;
};

class TestSetupView : public virtual TestSetupOutputView,
                      public virtual TestSetupInputView {};

class TestSetupController {
  public:
    class EventListener {
      public:
        virtual ~EventListener() = default;
        virtual void notifyThatUserHasSelectedTestSettingsFile(
            const std::string &) = 0;
    };
    virtual ~TestSetupController() = default;
    virtual void subscribe(SessionController *) = 0;
    virtual void subscribe(EventListener *) = 0;
};

class TestSetupPresenter : public virtual TestSetupController::EventListener,
                           public virtual PresenterSimple {};
}

#endif
