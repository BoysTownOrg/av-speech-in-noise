#ifndef AV_SPEECH_IN_NOISE_MACOS_APPKITVIEW_H_
#define AV_SPEECH_IN_NOISE_MACOS_APPKITVIEW_H_

#include <av-speech-in-noise/Interface.hpp>
#include <av-speech-in-noise/ui/Emotion.hpp>
#include <av-speech-in-noise/ui/Consonant.hpp>
#include <av-speech-in-noise/ui/CoordinateResponseMeasure.hpp>
#include <av-speech-in-noise/ui/Subject.hpp>

#import <AppKit/AppKit.h>

#include <map>
#include <vector>
#include <unordered_map>

@class CoordinateResponseMeasureUIActions;
@class ConsonantUIActions;
@class ObjCToCppAction;
@class ClickTrackingButton;

namespace av_speech_in_noise {
class ObjCToCppResponder {
  public:
    AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(ObjCToCppResponder);
    virtual void callback(id sender) = 0;
};

class SubjectAppKitView : public SubjectView {
  public:
    explicit SubjectAppKitView(NSWindow *window) : window{window} {}

    void moveToScreen(int index) override {
        [window setFrame:[NSScreen.screens objectAtIndex:index].frame
                 display:YES];
    }

    void show() override { [window makeKeyAndOrderFront:nil]; }

    void hide() override { [window orderOut:nil]; }

  private:
    NSWindow *window;
};

namespace submitting_emotion {
class AppKitUI : public UI {
  public:
    explicit AppKitUI(NSView *);
    void attach(Observer *) override;
    auto emotion() -> Emotion override;
    auto playButton() -> View & override;
    auto responseButtons() -> View & override;
    auto cursor() -> View & override;
    void show() override;
    void hide() override;
    void populateResponseButtons(
        const std::vector<std::vector<Emotion>> &) override;

    class PlayButton : public View, public ObjCToCppResponder {
      public:
        explicit PlayButton(NSView *);
        void attach(Observer *);
        void show() override;
        void hide() override;
        void callback(id sender) override;

      private:
        // order important
        ObjCToCppAction *action;
        NSButton *button;
        Observer *observer{};
    };

    class ResponseButtons : public View, public ObjCToCppResponder {
      public:
        ResponseButtons();
        void populate(NSView *, const std::vector<std::vector<Emotion>> &);
        void attach(Observer *);
        void show() override;
        void hide() override;
        void callback(id sender) override;
        auto emotion() -> Emotion;

      private:
        // order important
        std::map<void *, Emotion> emotions{};
        NSButton *lastButtonPressed{};
        Observer *observer{};
        ObjCToCppAction *action;
        NSStackView *buttons;
    };

    class Cursor : public View {
      public:
        void show() override;
        void hide() override;
    };

  private:
    PlayButton playButton_;
    ResponseButtons responseButtons_;
    Cursor cursor_;
    NSView *view;
    Observer *observer{};
};
}

namespace submitting_consonant {
class AppKitUI : public View, public Control {
  public:
    explicit AppKitUI(NSView *);
    void attach(Observer *) override;
    void show() override;
    void hide() override;
    void showResponseButtons() override;
    void hideResponseButtons() override;
    void showReadyButton() override;
    void hideReadyButton() override;
    auto consonant() -> Consonant override;
    auto buttonPressedSeconds() -> double override;
    void hideCursor() override;
    void notifyThatResponseButtonHasBeenClicked(id sender);
    void notifyThatReadyButtonHasBeenClicked();
    void showCursor() override;

  private:
    std::unordered_map<void *, Consonant> consonants;
    NSView *view;
    NSStackView *responseButtons;
    NSButton *readyButton{};
    ClickTrackingButton *lastButtonPressed{};
    ConsonantUIActions *actions;
    Observer *listener_{};
};
}

class AppKitCoordinateResponseMeasureUI
    : public CoordinateResponseMeasureControl,
      public CoordinateResponseMeasureView {
  public:
    explicit AppKitCoordinateResponseMeasureUI(NSView *);
    auto numberResponse() -> std::string override;
    auto greenResponse() -> bool override;
    auto blueResponse() -> bool override;
    auto whiteResponse() -> bool override;
    void showResponseButtons() override;
    void hideResponseButtons() override;
    void showNextTrialButton() override;
    void hideNextTrialButton() override;
    void attach(Observer *) override;
    void show() override;
    void hide() override;
    void notifyThatResponseButtonHasBeenClicked(id sender);
    void notifyThatReadyButtonHasBeenClicked();

  private:
    void addNextTrialButton();
    auto lastPressedColor() -> NSColor *;
    void addNumberButton(NSColor *color, int number, int row, std::size_t col);
    void addButtonRow(NSColor *color, int row);

    NSView *view;
    NSStackView *responseButtons{};
    NSButton *nextTrialButton;
    NSButton *lastButtonPressed{};
    CoordinateResponseMeasureUIActions *actions;
    Observer *listener_{};
};
}

#endif
