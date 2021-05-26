#include "../run.h"
#include "../AppKitView.h"
#include "../AppKit-utility.h"
#include "../Foundation-utility.h"
#include "../objective-c-bridge.h"
#include "../objective-c-adapters.h"
#include "../TobiiProEyeTracker.hpp"
#include <av-speech-in-noise/ui/EyeTrackerCalibration.hpp>
#include <av-speech-in-noise/core/EyeTrackerCalibration.hpp>
#import <AppKit/AppKit.h>
#include <vector>
#include <algorithm>

@interface AvSpeechInNoiseEyeTrackerMenuObserverImpl
    : NSObject <EyeTrackerMenuObserver>
@end

@implementation AvSpeechInNoiseEyeTrackerMenuObserverImpl {
  @public
    av_speech_in_noise::eye_tracker_calibration::Control::Observer
        *calibrationObserver;
    av_speech_in_noise::eye_tracker_calibration::validation::Control::Observer
        *calibrationValidationObserver;
}

- (void)notifyThatRunCalibrationHasBeenClicked {
    calibrationObserver->notifyThatMenuHasBeenSelected();
}

- (void)notifyThatRunCalibrationValidationHasBeenClicked {
    calibrationValidationObserver->notifyThatMenuHasBeenSelected();
}
@end

@interface AvSpeechInNoiseEyeTrackerCalibrationTesterViewActions : NSObject
@end

@implementation AvSpeechInNoiseEyeTrackerCalibrationTesterViewActions {
  @public
    av_speech_in_noise::eye_tracker_calibration::Control::Observer *observer;
}

- (void)notifyThatSubmitButtonHasBeenClicked {
    observer->notifyThatSubmitButtonHasBeenClicked();
}
@end

@interface AvSpeechInNoiseEyeTrackingCalibrationValidationMenuObserverImpl
    : NSObject <AvSpeechInNoiseEyeTrackingCalibrationValidationMenuObserver>
@end

@implementation
    AvSpeechInNoiseEyeTrackingCalibrationValidationMenuObserverImpl {
  @public
    av_speech_in_noise::eye_tracker_calibration::validation::Control::Observer
        *observer;
}

- (void)notifyThatRunHasBeenClicked {
    observer->notifyThatMenuHasBeenSelected();
}
@end

@interface AvSpeechInNoiseAppKitCircleView : NSView
@end

@implementation AvSpeechInNoiseAppKitCircleView
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

@interface AvSpeechInNoiseEyeTrackerCalibrationView : NSView
@end

@implementation AvSpeechInNoiseEyeTrackerCalibrationView {
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

@interface AvSpeechInNoiseEyeTrackerCalibrationAppKitAnimationDelegate
    : NSObject <NSAnimationDelegate>
@end

@implementation AvSpeechInNoiseEyeTrackerCalibrationAppKitAnimationDelegate {
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

    explicit AppKitSubjectView(NSWindow *subjectWindow)
        : circleView{[[AvSpeechInNoiseAppKitCircleView alloc]
              initWithFrame:NSMakeRect(0, 0, normalDotDiameterPoints,
                                normalDotDiameterPoints)]},
          animationDelegate{
              [[AvSpeechInNoiseEyeTrackerCalibrationAppKitAnimationDelegate
                  alloc] init]},
          subjectWindow{subjectWindow} {
        [subjectWindow.contentViewController.view addSubview:circleView];
    }

    void attach(SubjectView::Observer *a) override {
        animationDelegate->observer = a;
    }

    void moveDotTo(WindowPoint point) override {
        animate(circleView,
            NSMakeRect(point.x * circleView.superview.frame.size.width -
                    circleView.frame.size.width / 2,
                point.y * circleView.superview.frame.size.height -
                    circleView.frame.size.height / 2,
                circleView.frame.size.width, circleView.frame.size.height),
            1.5, animationDelegate);
    }

    void shrinkDot() override {
        animate(circleView,
            NSMakeRect(circleView.frame.origin.x +
                    (circleView.frame.size.width - shrunkenDotDiameterPoints) /
                        2,
                circleView.frame.origin.y +
                    (circleView.frame.size.height - shrunkenDotDiameterPoints) /
                        2,
                shrunkenDotDiameterPoints, shrunkenDotDiameterPoints),
            0.5, animationDelegate);
    }

    void growDot() override {
        animate(circleView,
            NSMakeRect(circleView.frame.origin.x +
                    (circleView.frame.size.width - normalDotDiameterPoints) / 2,
                circleView.frame.origin.y +
                    (circleView.frame.size.height - normalDotDiameterPoints) /
                        2,
                normalDotDiameterPoints, normalDotDiameterPoints),
            0.5, animationDelegate);
    }

    void show() override { [subjectWindow makeKeyAndOrderFront:nil]; }

    void hide() override { [subjectWindow orderOut:nil]; }

  private:
    AvSpeechInNoiseAppKitCircleView *circleView;
    AvSpeechInNoiseEyeTrackerCalibrationAppKitAnimationDelegate
        *animationDelegate;
    NSWindow *subjectWindow;
};

class AppKitTesterUI : public TesterView, public Control {
  public:
    explicit AppKitTesterUI(NSWindow *testerWindow,
        AvSpeechInNoiseEyeTrackerMenuObserverImpl *eyeTrackingMenuObserver)
        : subjectView{[[AvSpeechInNoiseEyeTrackerCalibrationView alloc] init]},
          actions{[[AvSpeechInNoiseEyeTrackerCalibrationTesterViewActions
              alloc] init]},
          eyeTrackingMenuObserver{eyeTrackingMenuObserver}, testerWindow{
                                                                testerWindow} {
        const auto submitButton {
            nsButton("confirm", actions,
                @selector(notifyThatSubmitButtonHasBeenClicked))
        };
        addAutolayoutEnabledSubview(
            testerWindow.contentViewController.view, subjectView);
        addAutolayoutEnabledSubview(
            testerWindow.contentViewController.view, submitButton);
        [NSLayoutConstraint activateConstraints:@[
            [subjectView.leadingAnchor
                constraintEqualToAnchor:testerWindow.contentViewController.view
                                            .leadingAnchor],
            [subjectView.trailingAnchor
                constraintEqualToAnchor:testerWindow.contentViewController.view
                                            .trailingAnchor],
            [subjectView.topAnchor
                constraintEqualToAnchor:testerWindow.contentViewController.view
                                            .topAnchor],
            [subjectView.bottomAnchor
                constraintEqualToAnchor:submitButton.topAnchor],
            [submitButton.trailingAnchor
                constraintEqualToAnchor:testerWindow.contentViewController.view
                                            .trailingAnchor
                               constant:-8],
            [submitButton.bottomAnchor
                constraintEqualToAnchor:testerWindow.contentViewController.view
                                            .bottomAnchor
                               constant:-8]
        ]];
    }

    void attach(Observer *a) override {
        subjectView->observer = a;
        eyeTrackingMenuObserver->calibrationObserver = a;
        actions->observer = a;
    }

    void drawRed(Line line) override {
        subjectView->redLines.push_back(line);
        subjectView.needsDisplay = YES;
    }

    void drawGreen(Line line) override {
        subjectView->greenLines.push_back(line);
        subjectView.needsDisplay = YES;
    }

    void drawWhiteCircleWithCenter(WindowPoint point) override {
        subjectView->whiteCircleCenters.push_back(point);
        subjectView.needsDisplay = YES;
    }

    auto whiteCircleCenters() -> std::vector<WindowPoint> override {
        return subjectView->whiteCircleCenters;
    }

    auto whiteCircleDiameter() -> double override {
        return 25 / subjectView.frame.size.width;
    }

    void clear() override {
        subjectView->redLines.clear();
        subjectView->greenLines.clear();
        subjectView->whiteCircleCenters.clear();
    }

    void show() override { [testerWindow makeKeyAndOrderFront:nil]; }

    void hide() override { [testerWindow orderOut:nil]; }

  private:
    AvSpeechInNoiseEyeTrackerCalibrationView *subjectView;
    AvSpeechInNoiseEyeTrackerCalibrationTesterViewActions *actions;
    AvSpeechInNoiseEyeTrackerMenuObserverImpl *eyeTrackingMenuObserver;
    NSWindow *testerWindow;
};
}

namespace validation {
class AppKitTesterView : public TesterView {
  public:
    explicit AppKitTesterView(
        NSObject<AvSpeechInNoiseCalibrationValidationTesterView> *view)
        : view{view} {}

    void setLeftEyeAccuracyDegrees(const std::string &s) override {
        [view setLeftEyeAccuracyDegrees:nsString(s)];
    }

    void setLeftEyePrecisionDegrees(const std::string &s) override {
        [view setLeftEyePrecisionDegrees:nsString(s)];
    }

    void setRightEyeAccuracyDegrees(const std::string &s) override {
        [view setRightEyeAccuracyDegrees:nsString(s)];
    }

    void setRightEyePrecisionDegrees(const std::string &s) override {
        [view setRightEyePrecisionDegrees:nsString(s)];
    }

    void show() override { [view show]; }

    void hide() override { [view hide]; }

  private:
    NSObject<AvSpeechInNoiseCalibrationValidationTesterView> *view;
};

class AppKitControl : public Control {
  public:
    explicit AppKitControl(AvSpeechInNoiseEyeTrackerMenuObserverImpl *observer)
        : observer{observer} {}

    void attach(Observer *a) override {
        observer->calibrationValidationObserver = a;
    }

  private:
    AvSpeechInNoiseEyeTrackerMenuObserverImpl *observer;
};
}
}

namespace av_speech_in_noise {
static void main(NSObject<TestSetupUIFactory> *testSetupUIFactory,
    NSObject<SessionUI> *sessionUI, NSObject<TestUI> *testUI,
    NSObject<FreeResponseUI> *freeResponseUI,
    NSObject<SyllablesUI> *syllablesUI,
    NSObject<ChooseKeywordsUI> *chooseKeywordsUI,
    NSObject<CorrectKeywordsUI> *correctKeywordsUI,
    NSObject<PassFailUI> *passFailUI,
    NSObject<EyeTrackerRunMenu> *eyeTrackerMenu,
    NSObject<AvSpeechInNoiseCalibrationValidationTesterView>
        *calibrationValidationTesterView) {
    static TobiiProTracker eyeTracker;
    static TestSetupUIFactoryImpl testSetupViewFactory{testSetupUIFactory};
    static DefaultOutputFileNameFactory outputFileNameFactory;
    static SessionUIImpl sessionUIAdapted{sessionUI};
    static TestUIImpl testUIAdapted{testUI};
    static FreeResponseUIImpl freeResponseUIAdapted{freeResponseUI};
    static SyllablesUIImpl syllablesUIAdapted{syllablesUI};
    static ChooseKeywordsUIImpl chooseKeywordsUIAdapted{chooseKeywordsUI};
    static CorrectKeywordsUIImpl correctKeywordsUIAdapted{correctKeywordsUI};
    static PassFailUIImpl passFailUIAdapted{passFailUI};
    const auto calibrationViewController{
        nsTabViewControllerWithoutTabControl()};
    const auto subjectScreen{[[NSScreen screens] lastObject]};
    const auto subjectScreenFrame{subjectScreen.frame};
    const auto testerScreen{[[NSScreen screens] firstObject]};
    const auto testerScreenFrame{testerScreen.frame};
    calibrationViewController.view.frame = subjectScreenFrame;
    const auto animatingWindow{
        [NSWindow windowWithContentViewController:calibrationViewController]};
    [animatingWindow setStyleMask:NSWindowStyleMaskBorderless];
    [animatingWindow setFrame:subjectScreenFrame display:YES];
    const auto calibrationResultsViewController{
        nsTabViewControllerWithoutTabControl()};
    calibrationResultsViewController.view.frame = testerScreenFrame;
    const auto calibrationResultsWindow{[NSWindow
        windowWithContentViewController:calibrationResultsViewController]};
    calibrationResultsWindow.styleMask = NSWindowStyleMaskBorderless;
    [calibrationResultsWindow setFrame:testerScreenFrame display:YES];
    animatingWindow.level = NSScreenSaverWindowLevel;
    calibrationResultsWindow.level = NSScreenSaverWindowLevel;
    const auto eyeTrackingMenuObserver{
        [[AvSpeechInNoiseEyeTrackerMenuObserverImpl alloc] init]};
    [eyeTrackerMenu attach:eyeTrackingMenuObserver];
    static eye_tracker_calibration::validation::AppKitTesterView
        eyeTrackingCalibrationValidationTesterViewAdapted{
            calibrationValidationTesterView};
    static eye_tracker_calibration::AppKitSubjectView
        eyeTrackerCalibrationSubjectView{animatingWindow};
    static eye_tracker_calibration::AppKitTesterUI
        eyeTrackerCalibrationTesterUI{
            calibrationResultsWindow, eyeTrackingMenuObserver};
    static eye_tracker_calibration::SubjectPresenterImpl
        eyeTrackerCalibrationSubjectPresenter{eyeTrackerCalibrationSubjectView};
    static eye_tracker_calibration::SubjectPresenterImpl
        eyeTrackerCalibrationValidationSubjectPresenter{
            eyeTrackerCalibrationSubjectView};
    static eye_tracker_calibration::TesterPresenterImpl
        eyeTrackerCalibrationTesterPresenter{eyeTrackerCalibrationTesterUI};
    static eye_tracker_calibration::validation::TesterPresenterImpl
        eyeTrackingCalibrationValidationTesterPresenter{
            eyeTrackingCalibrationValidationTesterViewAdapted};
    static auto eyeTrackingCalibrationValidationTobiiProValidator{
        eyeTracker.calibrationValidator()};
    static eye_tracker_calibration::validation::InteractorImpl
        eyeTrackingCalibrationValidationInteractor{
            eyeTrackerCalibrationValidationSubjectPresenter,
            eyeTrackingCalibrationValidationTesterPresenter,
            eyeTrackingCalibrationValidationTobiiProValidator,
            {{0.5, 0.5}, {0.3F, 0.3F}, {0.3F, 0.7F}, {0.7F, 0.3F},
                {0.7F, 0.7F}}};
    static auto calibrator{eyeTracker.calibrator()};
    static eye_tracker_calibration::InteractorImpl
        eyeTrackerCalibrationInteractor{eyeTrackerCalibrationSubjectPresenter,
            eyeTrackerCalibrationTesterPresenter, calibrator,
            {{0.5, 0.5}, {0.1F, 0.1F}, {0.1F, 0.9F}, {0.9F, 0.1F},
                {0.9F, 0.9F}}};
    static eye_tracker_calibration::Controller eyeTrackerCalibrationController{
        eyeTrackerCalibrationTesterUI, eyeTrackerCalibrationInteractor};
    static eye_tracker_calibration::validation::AppKitControl
        eyeTrackingCalibrationValidationControl{eyeTrackingMenuObserver};
    static eye_tracker_calibration::validation::Controller
        eyeTrackingCalibrationValidationController{
            eyeTrackingCalibrationValidationControl,
            eyeTrackingCalibrationValidationInteractor};
    initializeAppAndRunEventLoop(eyeTracker, outputFileNameFactory,
        testSetupViewFactory, sessionUIAdapted, testUIAdapted,
        freeResponseUIAdapted, syllablesUIAdapted, chooseKeywordsUIAdapted,
        correctKeywordsUIAdapted, passFailUIAdapted);
}
}

@implementation AvSpeechInNoiseMain
+ (void)withTobiiPro:(NSObject<TestSetupUIFactory> *)testSetupUIFactory
                          withSessionUI:(NSObject<SessionUI> *)sessionUI
                             withTestUI:(NSObject<TestUI> *)testUI
                     withFreeResponseUI:
                         (NSObject<FreeResponseUI> *)freeResponseUI
                        withSyllablesUI:(NSObject<SyllablesUI> *)syllablesUI
                   withChooseKeywordsUI:
                       (NSObject<ChooseKeywordsUI> *)chooseKeywordsUI
                  withCorrectKeywordsUI:
                      (NSObject<CorrectKeywordsUI> *)correctKeywordsUI
                         withPassFailUI:(NSObject<PassFailUI> *)passFailUI
                     withEyeTrackerMenu:
                         (NSObject<EyeTrackerRunMenu> *)eyeTrackerMenu
    withCalibrationValidationTesterView:
        (NSObject<AvSpeechInNoiseCalibrationValidationTesterView> *)
            calibrationValidationTesterView {
    av_speech_in_noise::main(testSetupUIFactory, sessionUI, testUI,
        freeResponseUI, syllablesUI, chooseKeywordsUI, correctKeywordsUI,
        passFailUI, eyeTrackerMenu, calibrationValidationTesterView);
}
@end
