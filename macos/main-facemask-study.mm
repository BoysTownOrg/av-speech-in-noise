#include "main.h"
#include "MacOsTestSetupViewFactory.h"
#include "common-objc.h"
#include <presentation/SessionControllerImpl.hpp>
#include <presentation/TestSettingsInterpreter.hpp>
#import <AppKit/AppKit.h>
#include <string>
#include <vector>
#include <algorithm>
#include <fstream>
#include <sstream>

@class FacemaskStudySetupViewActions;

namespace av_speech_in_noise {
struct ConditionSelection {
    NSButton *button;
    LocalUrl url;
};

class FacemaskStudySetupView : public TestSetupUI {
  public:
    explicit FacemaskStudySetupView(NSViewController *);
    void show() override;
    void hide() override;
    auto testSettingsFile() -> std::string override;
    void setTestSettingsFile(std::string) override;
    void attach(Observer *) override;
    void populateTransducerMenu(std::vector<std::string>) override {}
    auto startingSnr() -> std::string override { return "0"; }
    auto testerId() -> std::string override { return {}; }
    auto subjectId() -> std::string override {
        return subjectIdField.stringValue.UTF8String;
    }
    auto session() -> std::string override { return {}; }
    auto rmeSetting() -> std::string override { return {}; }
    auto transducer() -> std::string override { return {}; }
    void notifyThatConfirmButtonHasBeenClicked();
    void notifyThatPlayLeftSpeakerCalibrationButtonHasBeenClicked();
    void notifyThatPlayRightSpeakerCalibrationButtonHasBeenClicked();

  private:
    std::vector<ConditionSelection> conditionSelections;
    NSTextField *subjectIdField;
    FacemaskStudySetupViewActions *actions;
    Observer *listener_{};
    NSViewController *controller;
};

class FacemaskStudySetupViewFactory : public MacOsTestSetupViewFactory {
  public:
    auto make(NSViewController *c) -> std::unique_ptr<TestSetupUI> override {
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

- (void)notifyThatRadioButtonHasBeenClicked {
}

- (void)notifyThatConfirmButtonHasBeenClicked {
    controller->notifyThatConfirmButtonHasBeenClicked();
}

- (void)notifyThatPlayLeftSpeakerCalibrationButtonHasBeenClicked {
    controller->notifyThatPlayLeftSpeakerCalibrationButtonHasBeenClicked();
}

- (void)notifyThatPlayRightSpeakerCalibrationButtonHasBeenClicked {
    controller->notifyThatPlayRightSpeakerCalibrationButtonHasBeenClicked();
}
@end

namespace av_speech_in_noise {
class EyeTrackerStub : public EyeTracker {
    void allocateRecordingTimeSeconds(double) override {}
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
                                           [NSFont boldSystemFontOfSize:30],
                                       NSFontAttributeName,
                                       [NSColor colorWithRed:33. / 255
                                                       green:57. / 255
                                                        blue:98. / 255
                                                       alpha:1],
                                       NSForegroundColorAttributeName, nil]]];
}

static auto yellowColor() -> NSColor * {
    return [NSColor colorWithRed:250. / 255
                           green:216. / 255
                            blue:111. / 255
                           alpha:1];
}

static auto labelWithAttributedString(
    const std::string &s, NSColor *color, CGFloat size) -> NSTextField * {
    return [NSTextField
        labelWithAttributedString:
            [[NSAttributedString alloc]
                initWithString:nsString(s)
                    attributes:[NSDictionary dictionaryWithObjectsAndKeys:
                                                 [NSFont systemFontOfSize:size],
                                             NSFontAttributeName, color,
                                             NSForegroundColorAttributeName,
                                             nil]]];
}

static auto labelWithAttributedString(const std::string &s) -> NSTextField * {
    return labelWithAttributedString(s, yellowColor(), 36);
}

static auto verticalStackView(NSArray<NSView *> *views) -> NSStackView * {
    const auto view{[NSStackView stackViewWithViews:views]};
    view.orientation = NSUserInterfaceLayoutOrientationVertical;
    return view;
}

static auto labeledView(NSView *field, const std::string &s) -> NSStackView * {
    const auto label_{labelWithAttributedString(s, yellowColor(), 28)};
    [label_ setContentHuggingPriority:251
                       forOrientation:NSLayoutConstraintOrientationHorizontal];
    const auto stack { [NSStackView stackViewWithViews:@[ label_, field ]] };
    return stack;
}

static auto resourcePath(const std::string &stem, const std::string &extension)
    -> std::string {
    return [[NSBundle mainBundle] pathForResource:nsString(stem)
                                           ofType:nsString(extension)]
        .UTF8String;
}

static auto nsButtonArray(const std::vector<ConditionSelection> &v)
    -> NSArray<NSButton *> * {
    std::vector<NSButton *> buttons;
    buttons.reserve(v.size());
    for (const auto &x : v)
        buttons.push_back(x.button);
    return [NSArray arrayWithObjects:&buttons.front() count:buttons.size()];
}

static auto readContents(const LocalUrl &resourceUrl) -> std::string {
    std::ifstream file{resourceUrl.path};
    std::stringstream stream;
    stream << file.rdbuf();
    return stream.str();
}

static auto meta(const LocalUrl &resourceUrl) -> std::string {
    return TestSettingsInterpreterImpl::meta(readContents(resourceUrl));
}

static void push_back(std::vector<ConditionSelection> &conditionSelections,
    FacemaskStudySetupViewActions *actions, const std::string &stem) {
    conditionSelections.push_back(ConditionSelection {
        [NSButton radioButtonWithTitle : nsString(
            meta(LocalUrl{resourcePath(stem, "txt")})) target : actions action :
            @selector(notifyThatRadioButtonHasBeenClicked)],
            LocalUrl { resourcePath(stem, "txt") }
    });
}

FacemaskStudySetupView::FacemaskStudySetupView(NSViewController *controller)
    : subjectIdField{[NSTextField textFieldWithString:@""]},
      actions{[[FacemaskStudySetupViewActions alloc] init]}, controller{
                                                                 controller} {
    actions->controller = this;
    const auto titleLabel{[NSTextField
        labelWithAttributedString:
            [[NSAttributedString alloc]
                initWithString:@"Facemask Study"
                    attributes:[NSDictionary
                                   dictionaryWithObjectsAndKeys:
                                       [NSFont systemFontOfSize:40],
                                   NSFontAttributeName, [NSColor whiteColor],
                                   NSForegroundColorAttributeName, nil]]]};
    const auto instructionsLabel{
        labelWithAttributedString("Click browse to choose the session file.")};
    const auto confirmButton {
        button("", actions, @selector(notifyThatConfirmButtonHasBeenClicked))
    };
    const auto playLeftSpeakerCalibrationButton {
        button("play left speaker", actions,
            @selector(notifyThatPlayLeftSpeakerCalibrationButtonHasBeenClicked))
    };
    const auto playRightSpeakerCalibrationButton {
        button("play right speaker", actions,
            @selector
            (notifyThatPlayRightSpeakerCalibrationButtonHasBeenClicked))
    };
    setAttributedTitle(confirmButton, "START");
    confirmButton.wantsLayer = YES;
    confirmButton.bordered = NO;
    confirmButton.layer.cornerRadius = 8.0;
    [confirmButton.layer setBackgroundColor:[NSColor colorWithRed:114. / 255
                                                            green:172. / 255
                                                             blue:77. / 255
                                                            alpha:1]
                                                .CGColor];
    [controller.view.window setBackgroundColor:[NSColor colorWithRed:43. / 255
                                                               green:97. / 255
                                                                blue:198. / 255
                                                               alpha:1]];

    const auto logoImage{[NSImage imageNamed:@"btnrh.png"]};
    const auto logo{[NSImageView imageViewWithImage:logoImage]};
    logo.imageScaling = NSImageScaleProportionallyDown;
    logo.wantsLayer = YES;
    logo.layer.backgroundColor = NSColor.whiteColor.CGColor;
    push_back(conditionSelections, actions, "NoMask_AO");
    push_back(conditionSelections, actions, "NoMask_AV");
    push_back(conditionSelections, actions, "ClearMask_AO");
    push_back(conditionSelections, actions, "ClearMask_AV");
    push_back(conditionSelections, actions, "CommunicatorMask_AO");
    push_back(conditionSelections, actions, "CommunicatorMask_AV");
    push_back(conditionSelections, actions, "FabricMask_AO");
    push_back(conditionSelections, actions, "FabricMask_AV");
    push_back(conditionSelections, actions, "HospitalMask_AO");
    push_back(conditionSelections, actions, "HospitalMask_AV");
    const auto layoutStack {
        verticalStackView(@[
            [NSStackView stackViewWithViews:@[ logo, titleLabel ]],
            instructionsLabel, labeledView(subjectIdField, "Subject ID:"),
            verticalStackView(nsButtonArray(conditionSelections)), confirmButton
        ])
    };
    [subjectIdField setFont:[NSFont systemFontOfSize:30]];
    [subjectIdField setTextColor:NSColor.blackColor];
    subjectIdField.wantsLayer = YES;
    subjectIdField.layer.backgroundColor = NSColor.whiteColor.CGColor;
    [instructionsLabel
        setContentCompressionResistancePriority:751
                                 forOrientation:
                                     NSLayoutConstraintOrientationHorizontal];
    const auto playCalibrationButtonsStack {
        verticalStackView(@[
            playLeftSpeakerCalibrationButton, playRightSpeakerCalibrationButton
        ])
    };
    addAutolayoutEnabledSubview(controller.view, layoutStack);
    addAutolayoutEnabledSubview(controller.view, playCalibrationButtonsStack);
    for (const auto &x : conditionSelections)
        [NSLayoutConstraint activateConstraints:@[
            [conditionSelections.front().button.leadingAnchor
                constraintEqualToAnchor:x.button.leadingAnchor],
        ]];
    [NSLayoutConstraint activateConstraints:@[
        [layoutStack.topAnchor constraintEqualToAnchor:controller.view.topAnchor
                                              constant:8],
        [layoutStack.bottomAnchor
            constraintEqualToAnchor:playCalibrationButtonsStack.topAnchor
                           constant:-8],
        [layoutStack.leadingAnchor
            constraintEqualToAnchor:controller.view.leadingAnchor
                           constant:8],
        [layoutStack.trailingAnchor
            constraintEqualToAnchor:controller.view.trailingAnchor
                           constant:-8],
        [playCalibrationButtonsStack.trailingAnchor
            constraintEqualToAnchor:controller.view.trailingAnchor
                           constant:-8],
        [playCalibrationButtonsStack.bottomAnchor
            constraintEqualToAnchor:controller.view.bottomAnchor
                           constant:-8],
        [confirmButton.widthAnchor constraintEqualToConstant:1.4 *
                                   confirmButton.attributedTitle.size.width],
        [confirmButton.heightAnchor constraintEqualToConstant:1.5 *
                                    confirmButton.attributedTitle.size.height]
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
    auto found{std::find_if(conditionSelections.begin(),
        conditionSelections.end(), [](const ConditionSelection &selection) {
            return selection.button.state == NSControlStateValueOn;
        })};
    return found != conditionSelections.end() ? found->url.path : "";
}

void FacemaskStudySetupView::setTestSettingsFile(std::string) {}

void FacemaskStudySetupView::attach(Observer *e) { listener_ = e; }

void FacemaskStudySetupView::notifyThatConfirmButtonHasBeenClicked() {
    listener_->notifyThatConfirmButtonHasBeenClicked();
}

void FacemaskStudySetupView::
    notifyThatPlayLeftSpeakerCalibrationButtonHasBeenClicked() {
    listener_->notifyThatPlayLeftSpeakerCalibrationButtonHasBeenClicked();
}

void FacemaskStudySetupView::
    notifyThatPlayRightSpeakerCalibrationButtonHasBeenClicked() {
    listener_->notifyThatPlayRightSpeakerCalibrationButtonHasBeenClicked();
}

class MetaConditionOutputFileNameFactory : public OutputFileNameFactory {
  public:
    auto make(TimeStamp &timeStamp)
        -> std::unique_ptr<OutputFileName> override {
        return std::make_unique<MetaConditionOutputFileName>(timeStamp);
    }
};
}

int main() {
    av_speech_in_noise::EyeTrackerStub eyeTracker;
    av_speech_in_noise::FacemaskStudySetupViewFactory testSetupViewFactory;
    av_speech_in_noise::MetaConditionOutputFileNameFactory
        outputFileNameFactory;
    av_speech_in_noise::main(eyeTracker, testSetupViewFactory,
        outputFileNameFactory, "Desktop/check your data here");
}
