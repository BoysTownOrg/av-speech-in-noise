#include "../run.h"
#include "../AppKitView.h"
#include "../AppKit-utility.h"
#include "../Foundation-utility.h"
#include "../objective-c-bridge.h"
#include "../objective-c-adapters.h"
#include "../TobiiProEyeTracker.hpp"

#include <av-speech-in-noise/ui/EyeTrackerCalibration.hpp>
#include <av-speech-in-noise/core/EyeTrackerCalibration.hpp>
#include <av-speech-in-noise/ui/SubjectImpl.hpp>

#import <AppKit/AppKit.h>

#include <vector>
#include <algorithm>

@interface AvSpeechInNoiseEyeTrackerCalibrationControlObserverProxy : NSObject
@end

@interface AvSpeechInNoiseEyeTrackerCalibrationControlObserverProxy () <
    EyeTrackerMenuObserver>
@end

@implementation AvSpeechInNoiseEyeTrackerCalibrationControlObserverProxy {
  @public
    av_speech_in_noise::eye_tracker_calibration::Control::Observer *observer;
}

- (void)notifyThatRunCalibrationHasBeenClicked {
    observer->notifyThatMenuHasBeenSelected();
}

- (void)notifyThatSubmitButtonHasBeenClicked {
    observer->notifyThatSubmitButtonHasBeenClicked();
}
@end

@interface AvSpeechInNoiseEyeTrackerCalibrationValidationControlObserverProxy
    : NSObject
@end

@interface AvSpeechInNoiseEyeTrackerCalibrationValidationControlObserverProxy () <
    AvSpeechInNoiseEyeTrackerCalibrationValidationControlObserver>
@end

@implementation
    AvSpeechInNoiseEyeTrackerCalibrationValidationControlObserverProxy {
  @public
    av_speech_in_noise::eye_tracker_calibration::validation::Control::Observer
        *observer;
}

- (void)notifyThatCloseButtonHasBeenClicked {
    observer->notifyThatCloseButtonHasBeenClicked();
}

- (void)notifyThatMenuHasBeenSelected {
    observer->notifyThatMenuHasBeenSelected();
}
@end

@interface AvSpeechInNoiseCircleNSView : NSView
@end

@implementation AvSpeechInNoiseCircleNSView
- (void)drawRect:(NSRect)rect {
    NSBezierPath *thePath = [NSBezierPath bezierPath];
    [thePath appendBezierPathWithOvalInRect:rect];
    [[NSColor whiteColor] set];
    [thePath fill];
}
@end

namespace av_speech_in_noise::eye_tracker_calibration {
static void draw(NSRect rect, const std::vector<Line> &lines, NSColor *color) {
    NSBezierPath *path = [NSBezierPath bezierPath];
    for_each(lines.begin(), lines.end(), [=](const Line &line) {
        [path
            moveToPoint:NSMakePoint(line.a.x * rect.size.width + rect.origin.x,
                            line.a.y * rect.size.height + rect.origin.y)];
        [path
            lineToPoint:NSMakePoint(line.b.x * rect.size.width + rect.origin.x,
                            line.b.y * rect.size.height + rect.origin.y)];
    });
    [path closePath];
    [color set];
    [path stroke];
}
}

@interface AvSpeechInNoiseEyeTrackerCalibrationTesterNSView : NSView
@end

@implementation AvSpeechInNoiseEyeTrackerCalibrationTesterNSView {
  @public
    std::vector<av_speech_in_noise::eye_tracker_calibration::Line> redLines;
    std::vector<av_speech_in_noise::eye_tracker_calibration::Line> greenLines;
    std::vector<av_speech_in_noise::eye_tracker_calibration::WindowPoint>
        whiteCircleCenters;
    av_speech_in_noise::eye_tracker_calibration::Control::Observer *observer;
}

- (void)drawRect:(NSRect)rect {
    NSBezierPath *circlePath = [NSBezierPath bezierPath];
    for_each(whiteCircleCenters.begin(), whiteCircleCenters.end(),
        [=](const av_speech_in_noise::eye_tracker_calibration::WindowPoint
                &center) {
            [circlePath
                appendBezierPathWithOvalInRect:NSMakeRect(
                                                   center.x * rect.size.width -
                                                       25. / 2,
                                                   center.y * rect.size.height -
                                                       25. / 2,
                                                   25, 25)];
        });
    [[NSColor whiteColor] set];
    [circlePath fill];
    draw(rect, greenLines, [NSColor greenColor]);
    draw(rect, redLines, [NSColor redColor]);
}

- (BOOL)acceptsFirstResponder {
    return YES;
}

- (void)mouseUp:(NSEvent *)event {
    const auto mousePoint{[self convertPoint:[event locationInWindow]
                                    fromView:nil]};
    observer->notifyThatWindowHasBeenTouched(
        {mousePoint.x / self.frame.size.width,
            mousePoint.y / self.frame.size.height});
}
@end

@interface AvSpeechInNoiseEyeTrackerCalibrationSubjectViewNSAnimationDelegate
    : NSObject <NSAnimationDelegate>
@end

@implementation
    AvSpeechInNoiseEyeTrackerCalibrationSubjectViewNSAnimationDelegate {
  @public
    av_speech_in_noise::eye_tracker_calibration::SubjectView::Observer
        *observer;
}

- (void)animationDidEnd:(NSAnimation *)animation {
    observer->notifyThatAnimationHasFinished();
}
@end

namespace av_speech_in_noise::eye_tracker_calibration {
static void animate(NSView *view, NSRect endFrame, double durationSeconds,
    id<NSAnimationDelegate> delegate) {
    const auto mutableDictionary {
        [NSMutableDictionary
            dictionaryWithSharedKeySet:[NSDictionary sharedKeySetForKeys:@[
                NSViewAnimationTargetKey, NSViewAnimationStartFrameKey,
                NSViewAnimationEndFrameKey
            ]]]
    };
    [mutableDictionary setObject:view forKey:NSViewAnimationTargetKey];
    [mutableDictionary setObject:[NSValue valueWithRect:view.frame]
                          forKey:NSViewAnimationStartFrameKey];
    [mutableDictionary setObject:[NSValue valueWithRect:endFrame]
                          forKey:NSViewAnimationEndFrameKey];
    const auto viewAnimation{[[NSViewAnimation alloc]
        initWithViewAnimations:[NSArray
                                   arrayWithObjects:mutableDictionary, nil]]};
    [viewAnimation setDelegate:delegate];
    [viewAnimation setAnimationCurve:NSAnimationEaseInOut];
    viewAnimation.animationBlockingMode = NSAnimationNonblocking;
    [viewAnimation setDuration:durationSeconds];
    [viewAnimation startAnimation];
}

namespace {
class AppKitSubjectView : public SubjectView {
  public:
    static constexpr auto normalDotDiameterPoints{100};
    static constexpr auto shrunkenDotDiameterPoints{25};

    explicit AppKitSubjectView(NSView *view)
        : dot{[[AvSpeechInNoiseCircleNSView alloc]
              initWithFrame:NSMakeRect(0, 0, normalDotDiameterPoints,
                                normalDotDiameterPoints)]},
          animationDelegate{[
              [AvSpeechInNoiseEyeTrackerCalibrationSubjectViewNSAnimationDelegate
                  alloc] init]} {
        [view addSubview:dot];
        [dot setHidden:YES];
    }

    void attach(SubjectView::Observer *a) override {
        animationDelegate->observer = a;
    }

    void moveDotTo(WindowPoint point) override {
        animate(dot,
            NSMakeRect(point.x * dot.superview.frame.size.width -
                    dot.frame.size.width / 2,
                point.y * dot.superview.frame.size.height -
                    dot.frame.size.height / 2,
                dot.frame.size.width, dot.frame.size.height),
            1.5, animationDelegate);
    }

    void shrinkDot() override {
        animate(dot,
            NSMakeRect(dot.frame.origin.x +
                    (dot.frame.size.width - shrunkenDotDiameterPoints) / 2,
                dot.frame.origin.y +
                    (dot.frame.size.height - shrunkenDotDiameterPoints) / 2,
                shrunkenDotDiameterPoints, shrunkenDotDiameterPoints),
            0.5, animationDelegate);
    }

    void growDot() override {
        animate(dot,
            NSMakeRect(dot.frame.origin.x +
                    (dot.frame.size.width - normalDotDiameterPoints) / 2,
                dot.frame.origin.y +
                    (dot.frame.size.height - normalDotDiameterPoints) / 2,
                normalDotDiameterPoints, normalDotDiameterPoints),
            0.5, animationDelegate);
    }

    void show() override { [dot setHidden:NO]; }

    void hide() override { [dot setHidden:YES]; }

  private:
    AvSpeechInNoiseCircleNSView *dot;
    AvSpeechInNoiseEyeTrackerCalibrationSubjectViewNSAnimationDelegate
        *animationDelegate;
};

class AppKitTesterUI : public TesterView, public Control {
  public:
    explicit AppKitTesterUI(NSWindow *window,
        AvSpeechInNoiseEyeTrackerCalibrationControlObserverProxy *observerProxy)
        : view{[AvSpeechInNoiseEyeTrackerCalibrationTesterNSView new]},
          observerProxy{observerProxy}, window{window} {
        const auto submitButton {
            nsButton("confirm", observerProxy,
                @selector(notifyThatSubmitButtonHasBeenClicked))
        };
        addAutolayoutEnabledSubview(window.contentViewController.view, view);
        addAutolayoutEnabledSubview(
            window.contentViewController.view, submitButton);
        [NSLayoutConstraint activateConstraints:@[
            [view.leadingAnchor
                constraintEqualToAnchor:window.contentViewController.view
                                            .leadingAnchor],
            [view.trailingAnchor
                constraintEqualToAnchor:window.contentViewController.view
                                            .trailingAnchor],
            [view.topAnchor constraintEqualToAnchor:window.contentViewController
                                                        .view.topAnchor],
            [view.bottomAnchor constraintEqualToAnchor:submitButton.topAnchor],
            [submitButton.trailingAnchor
                constraintEqualToAnchor:window.contentViewController.view
                                            .trailingAnchor
                               constant:-8],
            [submitButton.bottomAnchor
                constraintEqualToAnchor:window.contentViewController.view
                                            .bottomAnchor
                               constant:-8]
        ]];
    }

    void attach(Observer *a) override {
        view->observer = a; // window mouseUp
        observerProxy->observer = a; // menu and submit button
    }

    void drawRed(Line line) override {
        view->redLines.push_back(line);
        view.needsDisplay = YES;
    }

    void drawGreen(Line line) override {
        view->greenLines.push_back(line);
        view.needsDisplay = YES;
    }

    void drawWhiteCircleWithCenter(WindowPoint point) override {
        view->whiteCircleCenters.push_back(point);
        view.needsDisplay = YES;
    }

    auto whiteCircleCenters() -> std::vector<WindowPoint> override {
        return view->whiteCircleCenters;
    }

    auto whiteCircleDiameter() -> double override {
        return 25 / view.frame.size.width;
    }

    void clear() override {
        view->redLines.clear();
        view->greenLines.clear();
        view->whiteCircleCenters.clear();
    }

    void show() override { [window makeKeyAndOrderFront:nil]; }

    void hide() override { [window orderOut:nil]; }

  private:
    AvSpeechInNoiseEyeTrackerCalibrationTesterNSView *view;
    AvSpeechInNoiseEyeTrackerCalibrationControlObserverProxy *observerProxy;
    NSWindow *window;
};
}

namespace validation {
class AppKitTesterUI : public TesterView, public Control {
  public:
    explicit AppKitTesterUI(
        NSObject<AvSpeechInNoiseEyeTrackerCalibrationValidationTesterUI> *ui)
        : ui{ui} {}

    void attach(Observer *a) override {
        const auto adapted{
            [[AvSpeechInNoiseEyeTrackerCalibrationValidationControlObserverProxy
                alloc] init]};
        adapted->observer = a;
        [ui attach:adapted];
    }

    void setLeftEyeAccuracyDegrees(const std::string &s) override {
        [ui setLeftEyeAccuracyDegrees:nsString(s)];
    }

    void setLeftEyePrecisionDegrees(const std::string &s) override {
        [ui setLeftEyePrecisionDegrees:nsString(s)];
    }

    void setRightEyeAccuracyDegrees(const std::string &s) override {
        [ui setRightEyeAccuracyDegrees:nsString(s)];
    }

    void setRightEyePrecisionDegrees(const std::string &s) override {
        [ui setRightEyePrecisionDegrees:nsString(s)];
    }

    void show() override { [ui show]; }

    void hide() override { [ui hide]; }

  private:
    NSObject<AvSpeechInNoiseEyeTrackerCalibrationValidationTesterUI> *ui;
};
}
}

namespace av_speech_in_noise {
namespace eye_tracker_calibration {
static void initialize(TobiiProTracker &tracker,
    NSObject<EyeTrackerRunMenu> *menu,
    NSObject<AvSpeechInNoiseEyeTrackerCalibrationValidationTesterUI>
        *validationTesterUI,
    av_speech_in_noise::SubjectPresenter &parentSubjectPresenter,
    NSWindow *subjectWindow) {
    const auto testerScreen{[[NSScreen screens] firstObject]};
    const auto testerScreenFrame{testerScreen.frame};
    const auto testerNSViewController{nsTabViewControllerWithoutTabControl()};
    testerNSViewController.view.frame = testerScreenFrame;
    const auto testerWindow{
        [NSWindow windowWithContentViewController:testerNSViewController]};
    testerWindow.styleMask = NSWindowStyleMaskBorderless;
    [testerWindow setFrame:testerScreenFrame display:YES];
    testerWindow.level = NSScreenSaverWindowLevel;
    const auto menuObserver{[
        [AvSpeechInNoiseEyeTrackerCalibrationControlObserverProxy alloc] init]};
    [menu attach:menuObserver];
    static validation::AppKitTesterUI validationTesterViewAdapter{
        validationTesterUI};
    static AppKitSubjectView subjectView{subjectWindow.contentView};
    static AppKitSubjectView validationSubjectView{subjectWindow.contentView};
    static AppKitTesterUI testerUI{testerWindow, menuObserver};
    static SubjectPresenterImpl subjectPresenter{
        subjectView, parentSubjectPresenter};
    static SubjectPresenterImpl validationSubjectPresenter{
        validationSubjectView, parentSubjectPresenter};
    static TesterPresenterImpl testerPresenter{testerUI};
    static validation::TesterPresenterImpl validationTesterPresenter{
        validationTesterViewAdapter};
    static auto validator{tracker.calibrationValidator()};
    static validation::InteractorImpl validationInteractor{
        validationSubjectPresenter, validationTesterPresenter, validator,
        {{0.5, 0.5}, {0.3F, 0.3F}, {0.3F, 0.7F}, {0.7F, 0.3F}, {0.7F, 0.7F}}};
    static auto calibrator{tracker.calibrator()};
    static InteractorImpl interactor{subjectPresenter, testerPresenter,
        calibrator,
        {{0.5, 0.5}, {0.1F, 0.1F}, {0.1F, 0.9F}, {0.9F, 0.1F}, {0.9F, 0.9F}}};
    static Controller controller{testerUI, interactor};
    static validation::Controller validationController{
        validationTesterViewAdapter, validationInteractor};
}
}

static void main(NSObject<TestSetupUIFactory> *testSetupUIFactory,
    NSObject<SessionUI> *sessionUI, NSObject<TestUI> *testUI,
    NSObject<FreeResponseUI> *freeResponseUI,
    NSObject<SyllablesUI> *syllablesUI,
    NSObject<ChooseKeywordsUI> *chooseKeywordsUI,
    NSObject<CorrectKeywordsUI> *correctKeywordsUI,
    NSObject<PassFailUI> *passFailUI,
    NSObject<EyeTrackerRunMenu> *eyeTrackerMenu,
    NSObject<AvSpeechInNoiseEyeTrackerCalibrationValidationTesterUI>
        *eyeTrackerCalibrationValidationTesterUI) {
    static TobiiProTracker eyeTracker;
    static TestSetupUIFactoryImpl testSetupViewFactory{testSetupUIFactory};
    static DefaultOutputFileNameFactory outputFileNameFactory;
    static SessionUIImpl sessionUIAdapter{sessionUI};
    static TestUIImpl testUIAdapter{testUI};
    static submitting_free_response::UIImpl freeResponseUIAdapter{
        freeResponseUI};
    static submitting_syllable::UIImpl syllablesUIAdapter{syllablesUI};
    static submitting_keywords::UIImpl chooseKeywordsUIAdapter{
        chooseKeywordsUI};
    static submitting_number_keywords::UIImpl correctKeywordsUIAdapter{
        correctKeywordsUI};
    static submitting_pass_fail::UIImpl passFailUIAdapter{passFailUI};
    const auto subjectWindow{av_speech_in_noise::subjectWindow()};
    static SubjectAppKitView subjectView{subjectWindow};
    static av_speech_in_noise::SubjectPresenterImpl subjectPresenter{
        subjectView, sessionUIAdapter};
    eye_tracker_calibration::initialize(eyeTracker, eyeTrackerMenu,
        eyeTrackerCalibrationValidationTesterUI, subjectPresenter,
        subjectWindow);
    initializeAppAndRunEventLoop(eyeTracker, outputFileNameFactory,
        testSetupViewFactory, sessionUIAdapter, testUIAdapter,
        freeResponseUIAdapter, syllablesUIAdapter, chooseKeywordsUIAdapter,
        correctKeywordsUIAdapter, passFailUIAdapter, subjectPresenter,
        subjectWindow);
}
}

@implementation AvSpeechInNoiseMain
+ (void)withTobiiPro:(NSObject<TestSetupUIFactory> *)testSetupUIFactory
                                  withSessionUI:(NSObject<SessionUI> *)sessionUI
                                     withTestUI:(NSObject<TestUI> *)testUI
                             withFreeResponseUI:
                                 (NSObject<FreeResponseUI> *)freeResponseUI
                                withSyllablesUI:
                                    (NSObject<SyllablesUI> *)syllablesUI
                           withChooseKeywordsUI:
                               (NSObject<ChooseKeywordsUI> *)chooseKeywordsUI
                          withCorrectKeywordsUI:
                              (NSObject<CorrectKeywordsUI> *)correctKeywordsUI
                                 withPassFailUI:
                                     (NSObject<PassFailUI> *)passFailUI
                             withEyeTrackerMenu:
                                 (NSObject<EyeTrackerRunMenu> *)eyeTrackerMenu
    withEyeTrackerCalibrationValidationTesterUI:
        (NSObject<AvSpeechInNoiseEyeTrackerCalibrationValidationTesterUI> *)
            eyeTrackerCalibrationValidationTesterUI {
    av_speech_in_noise::main(testSetupUIFactory, sessionUI, testUI,
        freeResponseUI, syllablesUI, chooseKeywordsUI, correctKeywordsUI,
        passFailUI, eyeTrackerMenu, eyeTrackerCalibrationValidationTesterUI);
}
@end
