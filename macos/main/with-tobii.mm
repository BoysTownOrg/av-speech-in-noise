#include "../run.h"
#include "../AppKitView.h"
#include "../AppKit-utility.h"
#include "../objective-c-bridge.h"
#include "../objective-c-adapters.h"
#include "../TobiiProEyeTracker.hpp"
#include <presentation/EyeTrackerCalibration.hpp>
#include <recognition-test/EyeTrackerCalibration.hpp>
#import <AppKit/AppKit.h>
#include <vector>
#include <algorithm>

@interface AvSpeechInNoiseEyeTrackerMenuObserverImpl
    : NSObject <EyeTrackerMenuObserver>
@end

@implementation AvSpeechInNoiseEyeTrackerMenuObserverImpl {
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
    av_speech_in_noise::eye_tracker_calibration::View::Observer *observer;
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
class AppKitUI : public View, public Control {
  public:
    static constexpr auto normalDotDiameterPoints{100};
    static constexpr auto shrunkenDotDiameterPoints{25};

    explicit AppKitUI(NSWindow *subjectWindow, NSWindow *testerWindow,
        NSObject<EyeTrackerRunMenu> *eyeTrackerMenu)
        : circleView{[[AvSpeechInNoiseAppKitCircleView alloc]
              initWithFrame:NSMakeRect(0, 0, normalDotDiameterPoints,
                                normalDotDiameterPoints)]},
          subjectView{[[AvSpeechInNoiseEyeTrackerCalibrationView alloc] init]},
          animationDelegate{
              [[AvSpeechInNoiseEyeTrackerCalibrationAppKitAnimationDelegate
                  alloc] init]},
          controlObserverProxy{
              [[AvSpeechInNoiseEyeTrackerMenuObserverImpl alloc] init]},
          testerWindow{testerWindow}, subjectWindow{subjectWindow} {
        [eyeTrackerMenu attach:controlObserverProxy];
        [subjectWindow.contentViewController.view addSubview:circleView];
        const auto submitButton {
            nsButton("confirm", controlObserverProxy,
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

    void attach(View::Observer *a) override { animationDelegate->observer = a; }

    void attach(Control::Observer *a) override {
        subjectView->observer = a;
        controlObserverProxy->observer = a;
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

    void show() override {
        [testerWindow makeKeyAndOrderFront:nil];
        [subjectWindow makeKeyAndOrderFront:nil];
    }

    void hide() override {
        [testerWindow orderOut:nil];
        [subjectWindow orderOut:nil];
    }

  private:
    AvSpeechInNoiseAppKitCircleView *circleView;
    AvSpeechInNoiseEyeTrackerCalibrationView *subjectView;
    AvSpeechInNoiseEyeTrackerCalibrationAppKitAnimationDelegate
        *animationDelegate;
    AvSpeechInNoiseEyeTrackerMenuObserverImpl *controlObserverProxy;
    NSWindow *testerWindow;
    NSWindow *subjectWindow;
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
    NSObject<EyeTrackerRunMenu> *eyeTrackerMenu) {
    static TobiiEyeTracker eyeTracker;
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
    static eye_tracker_calibration::AppKitUI eyeTrackerCalibrationView{
        animatingWindow, calibrationResultsWindow, eyeTrackerMenu};
    static eye_tracker_calibration::Presenter eyeTrackerCalibrationPresenter{
        eyeTrackerCalibrationView};
    static auto calibrator{eyeTracker.calibration()};
    static eye_tracker_calibration::Interactor eyeTrackerCalibrationInteractor{
        eyeTrackerCalibrationPresenter, calibrator,
        {{0.5, 0.5}, {0.1F, 0.1F}, {0.1F, 0.9F}, {0.9F, 0.1F}, {0.9F, 0.9F}}};
    static eye_tracker_calibration::Controller eyeTrackerCalibrationController{
        eyeTrackerCalibrationView, eyeTrackerCalibrationInteractor};
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
       withFreeResponseUI:(NSObject<FreeResponseUI> *)freeResponseUI
          withSyllablesUI:(NSObject<SyllablesUI> *)syllablesUI
     withChooseKeywordsUI:(NSObject<ChooseKeywordsUI> *)chooseKeywordsUI
    withCorrectKeywordsUI:(NSObject<CorrectKeywordsUI> *)correctKeywordsUI
           withPassFailUI:(NSObject<PassFailUI> *)passFailUI
       withEyeTrackerMenu:(NSObject<EyeTrackerRunMenu> *)eyeTrackerMenu {
    av_speech_in_noise::main(testSetupUIFactory, sessionUI, testUI,
        freeResponseUI, syllablesUI, chooseKeywordsUI, correctKeywordsUI,
        passFailUI, eyeTrackerMenu);
}
@end
