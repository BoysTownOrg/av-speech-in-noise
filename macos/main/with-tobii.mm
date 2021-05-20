#include "../run.h"
#include "../AppKitView.h"
#include "../AppKit-utility.h"
#include "../objective-c-bridge.h"
#include "../objective-c-adapters.h"
#include <av-speech-in-noise/Interface.hpp>
#include <presentation/EyeTrackerCalibration.hpp>
#include <recognition-test/EyeTrackerCalibration.hpp>
#import <AppKit/AppKit.h>
#include <exception>
#include <tobii_research.h>
#include <tobii_research_eyetracker.h>
#include <tobii_research_streams.h>
#include <tobii_research_calibration.h>
#include <gsl/gsl>
#include <exception>
#include <iterator>
#include <utility>
#include <vector>
#include <algorithm>
#include <cstddef>
#include <functional>

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
    for (const auto &line : lines) {
        [path
            moveToPoint:NSMakePoint(line.a.x * rect.size.width + rect.origin.x,
                            line.a.y * rect.size.height + rect.origin.y)];
        [path
            lineToPoint:NSMakePoint(line.b.x * rect.size.width + rect.origin.x,
                            line.b.y * rect.size.height + rect.origin.y)];
    }
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
    for (const auto &center : whiteCircleCenters) {
        [circlePath
            appendBezierPathWithOvalInRect:NSMakeRect(
                                               center.x * rect.size.width -
                                                   25. / 2,
                                               center.y * rect.size.height -
                                                   25. / 2,
                                               25, 25)];
    }
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

namespace av_speech_in_noise {
namespace eye_tracker_calibration {
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
          view{[[AvSpeechInNoiseEyeTrackerCalibrationView alloc] init]},
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
            testerWindow.contentViewController.view, view);
        addAutolayoutEnabledSubview(
            testerWindow.contentViewController.view, submitButton);
        [NSLayoutConstraint activateConstraints:@[
            [view.leadingAnchor
                constraintEqualToAnchor:testerWindow.contentViewController.view
                                            .leadingAnchor],
            [view.trailingAnchor
                constraintEqualToAnchor:testerWindow.contentViewController.view
                                            .trailingAnchor],
            [view.topAnchor
                constraintEqualToAnchor:testerWindow.contentViewController.view
                                            .topAnchor],
            [view.bottomAnchor constraintEqualToAnchor:submitButton.topAnchor],
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
        view->observer = a;
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
    AvSpeechInNoiseEyeTrackerCalibrationView *view;
    AvSpeechInNoiseEyeTrackerCalibrationAppKitAnimationDelegate
        *animationDelegate;
    AvSpeechInNoiseEyeTrackerMenuObserverImpl *controlObserverProxy;
    NSWindow *testerWindow;
    NSWindow *subjectWindow;
};
}
}

static auto eyeTracker(TobiiResearchEyeTrackers *eyeTrackers)
    -> TobiiResearchEyeTracker * {
    return eyeTrackers == nullptr || eyeTrackers->count == 0U
        ? nullptr
        : eyeTrackers->eyetrackers[0];
}

class TobiiEyeTracker : public EyeTracker {
  public:
    TobiiEyeTracker();
    ~TobiiEyeTracker() override;
    void allocateRecordingTimeSeconds(double s) override;
    void start() override;
    void stop() override;
    auto gazeSamples() -> BinocularGazeSamples override;
    auto currentSystemTime() -> EyeTrackerSystemTime override;

    class Calibration;
    class CalibrationValidation;

    auto calibration() -> Calibration {
        return Calibration{eyeTracker(eyeTrackers)};
    }

    class Address {
      public:
        explicit Address(TobiiResearchEyeTracker *eyetracker) {
            tobii_research_get_address(eyetracker, &address);
        }

        auto get() -> const char * { return address; }

        ~Address() { tobii_research_free_string(address); }

      private:
        char *address{};
    };

    class Calibration : public eye_tracker_calibration::EyeTrackerCalibrator {
      public:
        explicit Calibration(TobiiResearchEyeTracker *eyetracker)
            : eyetracker{eyetracker} {}

        void acquire() override {
            tobii_research_screen_based_calibration_enter_calibration_mode(
                eyetracker);
        }

        void release() override {
            tobii_research_screen_based_calibration_leave_calibration_mode(
                eyetracker);
        }

        void discard(eye_tracker_calibration::Point p) override {
            tobii_research_screen_based_calibration_discard_data(
                eyetracker, p.x, p.y);
        }

        void collect(eye_tracker_calibration::Point p) override {
            tobii_research_screen_based_calibration_collect_data(
                eyetracker, p.x, p.y);
        }

        auto results()
            -> std::vector<eye_tracker_calibration::Result> override {
            ComputeAndApply computeAndApply{eyetracker};
            return computeAndApply.results();
        }

        class ComputeAndApply;

        auto computeAndApply() -> ComputeAndApply {
            return ComputeAndApply{eyetracker};
        }

        class ComputeAndApply {
          public:
            explicit ComputeAndApply(TobiiResearchEyeTracker *eyetracker) {
                tobii_research_screen_based_calibration_compute_and_apply(
                    eyetracker, &result);
            }

            auto success() -> bool {
                return result != nullptr &&
                    result->status == TOBII_RESEARCH_CALIBRATION_SUCCESS;
            }

            auto results() -> std::vector<eye_tracker_calibration::Result> {
                if (result == nullptr)
                    return {};
                std::vector<eye_tracker_calibration::Result> results{
                    result->calibration_point_count};
                const gsl::span<TobiiResearchCalibrationPoint>
                    calibrationPoints{result->calibration_points,
                        result->calibration_point_count};
                std::transform(calibrationPoints.begin(),
                    calibrationPoints.end(), std::back_inserter(results),
                    [](const TobiiResearchCalibrationPoint &p) {
                        eye_tracker_calibration::Result transformedResult;
                        const gsl::span<TobiiResearchCalibrationSample>
                            calibrationSamples{p.calibration_samples,
                                p.calibration_sample_count};
                        std::transform(calibrationSamples.begin(),
                            calibrationSamples.end(),
                            std::back_inserter(
                                transformedResult.leftEyeMappedPoints),
                            [](const TobiiResearchCalibrationSample &sample) {
                                return eye_tracker_calibration::Point{
                                    sample.left_eye.position_on_display_area.x,
                                    sample.left_eye.position_on_display_area.y};
                            });
                        std::transform(calibrationSamples.begin(),
                            calibrationSamples.end(),
                            std::back_inserter(
                                transformedResult.rightEyeMappedPoints),
                            [](const TobiiResearchCalibrationSample &sample) {
                                return eye_tracker_calibration::Point{
                                    sample.right_eye.position_on_display_area.x,
                                    sample.right_eye.position_on_display_area
                                        .y};
                            });
                        transformedResult.point = {p.position_on_display_area.x,
                            p.position_on_display_area.y};
                        return transformedResult;
                    });

                return results;
            }

            ~ComputeAndApply() {
                tobii_research_free_screen_based_calibration_result(result);
            }

          private:
            TobiiResearchCalibrationResult *result{};
        };

      private:
        TobiiResearchEyeTracker *eyetracker{};
    };

  private:
    static void gaze_data_callback(
        TobiiResearchGazeData *gaze_data, void *self);
    void gazeDataReceived(TobiiResearchGazeData *gaze_data);

    std::vector<TobiiResearchGazeData> gazeData{};
    TobiiResearchEyeTrackers *eyeTrackers{};
    std::size_t head{};
};

TobiiEyeTracker::TobiiEyeTracker() {
    tobii_research_find_all_eyetrackers(&eyeTrackers);
}

TobiiEyeTracker::~TobiiEyeTracker() {
    tobii_research_free_eyetrackers(eyeTrackers);
}

void TobiiEyeTracker::allocateRecordingTimeSeconds(double seconds) {
    float gaze_output_frequency_Hz{};
    tobii_research_get_gaze_output_frequency(
        eyeTracker(eyeTrackers), &gaze_output_frequency_Hz);
    gazeData.resize(std::ceil(gaze_output_frequency_Hz * seconds) + 1);
    head = 0;
}

void TobiiEyeTracker::start() {
    tobii_research_subscribe_to_gaze_data(
        eyeTracker(eyeTrackers), gaze_data_callback, this);
}

void TobiiEyeTracker::stop() {
    tobii_research_unsubscribe_from_gaze_data(
        eyeTracker(eyeTrackers), gaze_data_callback);
}

void TobiiEyeTracker::gaze_data_callback(
    TobiiResearchGazeData *gaze_data, void *self) {
    static_cast<TobiiEyeTracker *>(self)->gazeDataReceived(gaze_data);
}

void TobiiEyeTracker::gazeDataReceived(TobiiResearchGazeData *gaze_data) {
    if (head < gazeData.size())
        gazeData.at(head++) = *gaze_data;
}

static auto at(std::vector<BinocularGazeSample> &b, gsl::index i)
    -> BinocularGazeSample & {
    return b.at(i);
}

static auto at(const std::vector<TobiiResearchGazeData> &b, gsl::index i)
    -> const TobiiResearchGazeData & {
    return b.at(i);
}

static auto eyeGaze(const TobiiResearchEyeData &d)
    -> const TobiiResearchNormalizedPoint2D & {
    return d.gaze_point.position_on_display_area;
}

static auto leftEyeGaze(const std::vector<TobiiResearchGazeData> &gaze,
    gsl::index i) -> const TobiiResearchNormalizedPoint2D & {
    return eyeGaze(at(gaze, i).left_eye);
}

static auto rightEyeGaze(const std::vector<TobiiResearchGazeData> &gaze,
    gsl::index i) -> const TobiiResearchNormalizedPoint2D & {
    return eyeGaze(at(gaze, i).right_eye);
}

static auto x(const TobiiResearchNormalizedPoint2D &p) -> float { return p.x; }

static auto y(const TobiiResearchNormalizedPoint2D &p) -> float { return p.y; }

static auto leftEyeGaze(std::vector<BinocularGazeSample> &b, gsl::index i)
    -> EyeGaze & {
    return at(b, i).left;
}

static auto rightEyeGaze(BinocularGazeSamples &b, gsl::index i) -> EyeGaze & {
    return at(b, i).right;
}

static auto x(EyeGaze &p) -> float & { return p.x; }

static auto y(EyeGaze &p) -> float & { return p.y; }

static auto size(const std::vector<BinocularGazeSample> &v) -> gsl::index {
    return v.size();
}

auto TobiiEyeTracker::gazeSamples() -> BinocularGazeSamples {
    BinocularGazeSamples gazeSamples_(head > 0 ? head - 1 : 0);
    for (gsl::index i{0}; i < size(gazeSamples_); ++i) {
        at(gazeSamples_, i).systemTime.microseconds =
            at(gazeData, i).system_time_stamp;
        x(leftEyeGaze(gazeSamples_, i)) = x(leftEyeGaze(gazeData, i));
        y(leftEyeGaze(gazeSamples_, i)) = y(leftEyeGaze(gazeData, i));
        x(rightEyeGaze(gazeSamples_, i)) = x(rightEyeGaze(gazeData, i));
        y(rightEyeGaze(gazeSamples_, i)) = y(rightEyeGaze(gazeData, i));
    }
    return gazeSamples_;
}

auto TobiiEyeTracker::currentSystemTime() -> EyeTrackerSystemTime {
    EyeTrackerSystemTime currentSystemTime{};
    int64_t microseconds = 0;
    tobii_research_get_system_time_stamp(&microseconds);
    currentSystemTime.microseconds = microseconds;
    return currentSystemTime;
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
        av_speech_in_noise::nsTabViewControllerWithoutTabControl()};
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
        av_speech_in_noise::nsTabViewControllerWithoutTabControl()};
    calibrationResultsViewController.view.frame = testerScreenFrame;
    const auto calibrationResultsWindow{[NSWindow
        windowWithContentViewController:calibrationResultsViewController]};
    calibrationResultsWindow.styleMask =
        NSWindowStyleMaskResizable | NSWindowStyleMaskTitled;
    [calibrationResultsWindow setFrame:testerScreenFrame display:YES];
    animatingWindow.level = NSScreenSaverWindowLevel;
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
