#include "main.h"
#include "MacOsTestSetupViewFactory.h"
#include "common-objc.h"
#include <presentation/Presenter.hpp>
#import <Cocoa/Cocoa.h>
#include <string>

@class FacemaskStudySetupViewActions;

namespace av_speech_in_noise {
class FacemaskStudySetupView : public View::TestSetup {
  public:
    explicit FacemaskStudySetupView(NSViewController *);
    void show() override;
    void hide() override;
    auto testSettingsFile() -> std::string override;
    void setTestSettingsFile(std::string) override;
    void subscribe(EventListener *) override;
    void populateTransducerMenu(std::vector<std::string>) override {}
    auto startingSnr() -> std::string override { return "0"; }
    auto testerId() -> std::string override { return {}; }
    auto subjectId() -> std::string override { return {}; }
    auto session() -> std::string override { return {}; }
    auto rmeSetting() -> std::string override { return {}; }
    auto transducer() -> std::string override { return {}; }
    void notifyThatConfirmButtonHasBeenClicked();
    void notifyThatBrowseForTestSettingsButtonHasBeenClicked();
    void notifyThatPlayCalibrationButtonHasBeenClicked();

  private:
    NSTextField *testSettingsField;
    FacemaskStudySetupViewActions *actions;
    EventListener *listener_{};
    NSViewController *controller;
};

class FacemaskStudySetupViewFactory : public MacOsTestSetupViewFactory {
  public:
    auto make(NSViewController *c)
        -> std::unique_ptr<View::TestSetup> override {
        return std::make_unique<FacemaskStudySetupView>(c);
    }
};
}

@interface FacemaskStudySetupViewActions : NSObject
@end

@implementation FacemaskStudySetupViewActions {
  @public
    av_speech_in_noise::FacemaskStudySetupView *controller;
}

- (void)notifyThatConfirmButtonHasBeenClicked {
    controller->notifyThatConfirmButtonHasBeenClicked();
}

- (void)notifyThatBrowseForTestSettingsButtonHasBeenClicked {
    controller->notifyThatBrowseForTestSettingsButtonHasBeenClicked();
}

- (void)notifyThatPlayCalibrationButtonHasBeenClicked {
    controller->notifyThatPlayCalibrationButtonHasBeenClicked();
}
@end

namespace av_speech_in_noise {
class EyeTrackerStub : public EyeTracker {
    void allocateRecordingTimeSeconds(double s) override {}
    void start() override {}
    void stop() override {}
    auto gazeSamples() -> BinocularGazeSamples override { return {}; }
    auto currentSystemTime() -> EyeTrackerSystemTime override { return {}; }
};

static auto button(const std::string &s, id target, SEL action) -> NSButton * {
    return [NSButton buttonWithTitle:nsString(s) target:target action:action];
}

static void addAutolayoutEnabledSubview(NSView *parent, NSView *child) {
    child.translatesAutoresizingMaskIntoConstraints = NO;
    [parent addSubview:child];
}

static void setAttributedTitle(NSButton *button, const std::string &s) {
    [button setAttributedTitle:
                [[NSAttributedString alloc]
                    initWithString:nsString(s)
                        attributes:[NSDictionary
                                       dictionaryWithObjectsAndKeys:
                                           [NSFont boldSystemFontOfSize:36],
                                       NSFontAttributeName,
                                       [NSColor colorWithRed:33. / 255
                                                       green:57. / 255
                                                        blue:98. / 255
                                                       alpha:1],
                                       NSForegroundColorAttributeName, nil]]];
}

static auto labelWithAttributedString(const std::string &s) -> NSTextField * {
    return [NSTextField
        labelWithAttributedString:
            [[NSAttributedString alloc]
                initWithString:nsString(s)
                    attributes:[NSDictionary dictionaryWithObjectsAndKeys:
                                                 [NSFont systemFontOfSize:36],
                                             NSFontAttributeName,
                                             [NSColor colorWithRed:250. / 255
                                                             green:216. / 255
                                                              blue:111. / 255
                                                             alpha:1],
                                             NSForegroundColorAttributeName,
                                             nil]]];
}

static auto verticalStackView(NSArray<NSView *> *views) -> NSStackView * {
    const auto view{[NSStackView stackViewWithViews:views]};
    view.orientation = NSUserInterfaceLayoutOrientationVertical;
    return view;
}

static auto labeledView(NSView *field, const std::string &s) -> NSStackView * {
    const auto label_{labelWithAttributedString(s)};
    [label_ setContentHuggingPriority:251
                       forOrientation:NSLayoutConstraintOrientationHorizontal];
    const auto stack { [NSStackView stackViewWithViews:@[ label_, field ]] };
    return stack;
}

FacemaskStudySetupView::FacemaskStudySetupView(NSViewController *controller)
    : testSettingsField{[NSTextField textFieldWithString:@""]},
      actions{[[FacemaskStudySetupViewActions alloc] init]}, controller{
                                                                 controller} {
    actions->controller = this;
    const auto titleLabel{[NSTextField
        labelWithAttributedString:
            [[NSAttributedString alloc]
                initWithString:@"Facemask Study"
                    attributes:[NSDictionary
                                   dictionaryWithObjectsAndKeys:
                                       [NSFont systemFontOfSize:36],
                                   NSFontAttributeName, [NSColor whiteColor],
                                   NSForegroundColorAttributeName, nil]]]};
    const auto instructionsLabel{
        labelWithAttributedString("Click browse to choose the session file.")};
    const auto browseForTestSettingsButton {
        button("", actions,
            @selector(notifyThatBrowseForTestSettingsButtonHasBeenClicked))
    };
    const auto confirmButton {
        button("", actions, @selector(notifyThatConfirmButtonHasBeenClicked))
    };
    const auto playCalibrationButton {
        button("play calibration", actions,
            @selector(notifyThatPlayCalibrationButtonHasBeenClicked))
    };

    setAttributedTitle(browseForTestSettingsButton, "Browse");
    setAttributedTitle(confirmButton, "START");
    confirmButton.bezelStyle = NSBezelStyleTexturedSquare;
    confirmButton.wantsLayer = YES;
    [confirmButton.layer setBackgroundColor:[NSColor colorWithRed:114. / 255
                                                            green:172. / 255
                                                             blue:77. / 255
                                                            alpha:1]
                                                .CGColor];
    browseForTestSettingsButton.bezelStyle = NSBezelStyleTexturedSquare;
    browseForTestSettingsButton.wantsLayer = YES;
    [browseForTestSettingsButton.layer
        setBackgroundColor:[NSColor colorWithRed:247. / 255
                                           green:191. / 255
                                            blue:44. / 255
                                           alpha:1]
                               .CGColor];
    [controller.view.window setBackgroundColor:[NSColor colorWithRed:43. / 255
                                                               green:97. / 255
                                                                blue:198. / 255
                                                               alpha:1]];

    const auto logo{
        [NSImageView imageViewWithImage:[NSImage imageNamed:@"b.bmp"]]};
    const auto layoutStack {
        verticalStackView(@[
            [NSStackView stackViewWithViews:@[ logo, titleLabel ]],
            instructionsLabel, browseForTestSettingsButton,
            labeledView(testSettingsField, "Session file:"), confirmButton
        ])
    };
    [instructionsLabel
        setContentCompressionResistancePriority:751
                                 forOrientation:
                                     NSLayoutConstraintOrientationHorizontal];
    addAutolayoutEnabledSubview(controller.view, layoutStack);
    addAutolayoutEnabledSubview(controller.view, playCalibrationButton);
    [NSLayoutConstraint activateConstraints:@[
        [layoutStack.topAnchor constraintEqualToAnchor:controller.view.topAnchor
                                              constant:8],
        [layoutStack.bottomAnchor
            constraintEqualToAnchor:playCalibrationButton.topAnchor
                           constant:-8],
        [layoutStack.leadingAnchor
            constraintEqualToAnchor:controller.view.leadingAnchor
                           constant:8],
        [layoutStack.trailingAnchor
            constraintEqualToAnchor:controller.view.trailingAnchor
                           constant:-8],
        [playCalibrationButton.trailingAnchor
            constraintEqualToAnchor:controller.view.trailingAnchor
                           constant:-8],
        [playCalibrationButton.bottomAnchor
            constraintEqualToAnchor:controller.view.bottomAnchor
                           constant:-8]
    ]];
}

void FacemaskStudySetupView::show() {
    [controller.view.window makeKeyAndOrderFront:nil];
    [controller.view setHidden:NO];
}

void FacemaskStudySetupView::hide() {
    [controller.view setHidden:YES];
    [controller.view.window orderOut:nil];
}

auto FacemaskStudySetupView::testSettingsFile() -> std::string {
    return testSettingsField.stringValue.UTF8String;
}

void FacemaskStudySetupView::setTestSettingsFile(std::string s) {
    [testSettingsField setStringValue:nsString(s)];
}

void FacemaskStudySetupView::subscribe(EventListener *e) { listener_ = e; }

void FacemaskStudySetupView::notifyThatConfirmButtonHasBeenClicked() {
    listener_->notifyThatConfirmButtonHasBeenClicked();
}

void FacemaskStudySetupView::
    notifyThatBrowseForTestSettingsButtonHasBeenClicked() {
    listener_->notifyThatBrowseForTestSettingsButtonHasBeenClicked();
}

void FacemaskStudySetupView::notifyThatPlayCalibrationButtonHasBeenClicked() {
    listener_->notifyThatPlayCalibrationButtonHasBeenClicked();
}
}

int main() {
    av_speech_in_noise::EyeTrackerStub eyeTracker;
    av_speech_in_noise::FacemaskStudySetupViewFactory testSetupViewFactory;
    av_speech_in_noise::main(eyeTracker, &testSetupViewFactory);
}
