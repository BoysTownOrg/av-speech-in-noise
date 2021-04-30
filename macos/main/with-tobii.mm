#include "../run.h"
#include "../AppKitView.h"
#include "../AppKit-utility.h"
#include <av-speech-in-noise/Interface.hpp>
#include <presentation/EyeTrackerCalibration.hpp>
#include <recognition-test/EyeTrackerCalibration.hpp>
#include <chrono>
#include <exception>
#include <iterator>
#include <tobii_research.h>
#include <tobii_research_eyetracker.h>
#include <tobii_research_streams.h>
#include <tobii_research_calibration.h>
#include <screen_based_calibration_validation.h>
#include <gsl/gsl>
#include <utility>
#include <vector>
#include <thread>
#include <algorithm>
#include <cstddef>
#import <AppKit/AppKit.h>

@interface CircleView : NSView
@end

@implementation CircleView
- (void)drawRect:(NSRect)rect {
    NSBezierPath *thePath = [NSBezierPath bezierPath];
    [thePath appendBezierPathWithOvalInRect:rect];
    [[NSColor whiteColor] set];
    [thePath fill];
}
@end

@interface AvSpeechInNoiseEyeTrackerCalibrationView : NSView
@end

namespace av_speech_in_noise::eye_tracker_calibration {
static void draw(NSRect rect, const std::vector<Line> &lines, NSColor *color) {
    NSBezierPath *path = [NSBezierPath bezierPath];
    for (const auto &line : lines) {
        [path
            moveToPoint:NSMakePoint(line.a.x * rect.size.width + rect.origin.x,
                            (1 - line.a.y) * rect.size.height + rect.origin.y)];
        [path
            lineToPoint:NSMakePoint(line.b.x * rect.size.width + rect.origin.x,
                            (1 - line.b.y) * rect.size.height + rect.origin.y)];
    }
    [path closePath];
    [color set];
    [path stroke];
}
}

@implementation AvSpeechInNoiseEyeTrackerCalibrationView {
  @public
    std::vector<av_speech_in_noise::eye_tracker_calibration::Line> redLines;
    std::vector<av_speech_in_noise::eye_tracker_calibration::Line> greenLines;
    std::vector<av_speech_in_noise::Point> whiteCircleCenters;
}
- (void)drawRect:(NSRect)rect {
    NSBezierPath *circlePath = [NSBezierPath bezierPath];
    for (const auto &center : whiteCircleCenters) {
        [circlePath
            appendBezierPathWithOvalInRect:NSMakeRect(
                                               center.x * rect.size.width -
                                                   25 / 2,
                                               (1 - center.y) *
                                                       rect.size.height -
                                                   25 / 2,
                                               25, 25)];
    }
    [[NSColor whiteColor] set];
    [circlePath fill];
    draw(rect, greenLines, [NSColor greenColor]);
    draw(rect, redLines, [NSColor redColor]);
}
@end

@interface AvSpeechInNoiseEyeTrackerCalibrationAnimationDelegate
    : NSObject <NSAnimationDelegate>
@end

@implementation AvSpeechInNoiseEyeTrackerCalibrationAnimationDelegate {
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
class AppKitView : public View {
  public:
    static constexpr auto normalDotDiameterPoints{100};
    static constexpr auto shrunkenDotDiameterPoints{25};

    explicit AppKitView(NSWindow *animatingWindow)
        : circleView{[[CircleView alloc]
              initWithFrame:NSMakeRect(0, 0, normalDotDiameterPoints,
                                normalDotDiameterPoints)]},
          view{[[AvSpeechInNoiseEyeTrackerCalibrationView alloc]
              initWithFrame:animatingWindow.frame]},
          delegate{[[AvSpeechInNoiseEyeTrackerCalibrationAnimationDelegate
              alloc] init]} {
        [animatingWindow.contentViewController.view addSubview:view];
        [view addSubview:circleView];
    }

    void attach(Observer *a) override { delegate->observer = a; }

    void moveDotTo(Point point) override {
        animate(circleView,
            NSMakeRect(point.x * view.frame.size.width -
                    circleView.frame.size.width / 2,
                (1 - point.y) * view.frame.size.height -
                    circleView.frame.size.height / 2,
                circleView.frame.size.width, circleView.frame.size.height),
            1.5, delegate);
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
            0.5, delegate);
    }

    void growDot() override {
        animate(circleView,
            NSMakeRect(circleView.frame.origin.x +
                    (circleView.frame.size.width - normalDotDiameterPoints) / 2,
                circleView.frame.origin.y +
                    (circleView.frame.size.height - normalDotDiameterPoints) /
                        2,
                normalDotDiameterPoints, normalDotDiameterPoints),
            0.5, delegate);
    }

    void drawRed(Line line) override { view->redLines.push_back(line); }

    void drawGreen(Line line) override { view->greenLines.push_back(line); }

    void drawWhiteCircleWithCenter(Point point) override {
        view->whiteCircleCenters.push_back(point);
    }

  private:
    CircleView *circleView;
    AvSpeechInNoiseEyeTrackerCalibrationView *view;
    AvSpeechInNoiseEyeTrackerCalibrationAnimationDelegate *delegate;
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

    auto calibrationValidation() -> CalibrationValidation {
        return CalibrationValidation{eyeTracker(eyeTrackers)};
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
            : eyetracker{eyetracker} {
            tobii_research_screen_based_calibration_enter_calibration_mode(
                eyetracker);
        }

        void discard(float x, float y) {
            tobii_research_screen_based_calibration_discard_data(
                eyetracker, x, y);
        }

        void calibrate(Point p) override {
            tobii_research_screen_based_calibration_collect_data(
                eyetracker, p.x, p.y);
        }

        auto results()
            -> std::vector<eye_tracker_calibration::Result> override {
            ComputeAndApply computeAndApply{eyetracker};
            return true
                ? std::vector<
                      eye_tracker_calibration::Result>{{{{0.51F, 0.502F},
                                                            {0.49F, 0.495F},
                                                            {0.48F, 0.503F},
                                                            {0.5, 0.49F},
                                                            {0.52F, 0.51F}},
                                                           {{0.51F, 0.503F},
                                                               {0.49F, 0.499F},
                                                               {0.48F, 0.513F},
                                                               {0.5, 0.493F},
                                                               {0.52F, 0.502F}},
                                                           {0.5, 0.5}},
                      {{{0.08F, 0.102F}, {0.09F, 0.095F}, {0.1F, 0.103F},
                           {0.11F, 0.09F}, {0.12F, 0.11F}},
                          {{0.08F, 0.102F}, {0.09F, 0.095F}, {0.1F, 0.103F},
                              {0.11F, 0.09F}, {0.12F, 0.11F}},
                          {0.1F, 0.1F}},
                      {{{0.08F, 0.902F}, {0.09F, 0.895F}, {0.1F, 0.903F},
                           {0.11F, 0.89F}, {0.12F, 0.91F}},
                          {{0.08F, 0.902F}, {0.09F, 0.895F}, {0.1F, 0.903F},
                              {0.11F, 0.89F}, {0.12F, 0.91F}},
                          {0.1F, 0.9F}},
                      {{{0.88F, 0.102F}, {0.89F, 0.095F}, {0.9F, 0.103F},
                           {0.91F, 0.09F}, {0.92F, 0.11F}},
                          {{0.88F, 0.102F}, {0.89F, 0.095F}, {0.9F, 0.103F},
                              {0.91F, 0.09F}, {0.92F, 0.11F}},
                          {0.9F, 0.1F}},
                      {{{0.88F, 0.902F}, {0.89F, 0.895F}, {0.9F, 0.903F},
                           {0.91F, 0.89F}, {0.92F, 0.91F}},
                          {{0.88F, 0.902F}, {0.89F, 0.895F}, {0.9F, 0.903F},
                              {0.91F, 0.89F}, {0.92F, 0.91F}},
                          {0.9F, 0.9F}}}
                : computeAndApply.results();
        }

        class ComputeAndApply;

        auto computeAndApply() -> ComputeAndApply {
            return ComputeAndApply{eyetracker};
        }

        ~Calibration() override {
            tobii_research_screen_based_calibration_leave_calibration_mode(
                eyetracker);
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
                                return Point{
                                    sample.left_eye.position_on_display_area.x,
                                    sample.left_eye.position_on_display_area.y};
                            });
                        std::transform(calibrationSamples.begin(),
                            calibrationSamples.end(),
                            std::back_inserter(
                                transformedResult.rightEyeMappedPoints),
                            [](const TobiiResearchCalibrationSample &sample) {
                                return Point{
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

    class CalibrationValidation {
      public:
        explicit CalibrationValidation(TobiiResearchEyeTracker *eyetracker) {
            Address address{eyetracker};
            if (tobii_research_screen_based_calibration_validation_init_default(
                    address.get(), &validator) ==
                CALIBRATION_VALIDATION_STATUS_INVALID_EYETRACKER)
                validator = nullptr;
        }

        class Enter;

        auto enter() -> Enter { return Enter{validator}; }

        ~CalibrationValidation() {
            if (validator != nullptr)
                tobii_research_screen_based_calibration_validation_destroy(
                    validator);
        }

        class Enter {
          public:
            explicit Enter(CalibrationValidator *validator)
                : validator{validator} {
                if (validator != nullptr)
                    tobii_research_screen_based_calibration_validation_enter_validation_mode(
                        validator);
            }

            void collect(float x, float y) {
                TobiiResearchNormalizedPoint2D point{x, y};
                if (validator != nullptr)
                    tobii_research_screen_based_calibration_validation_start_collecting_data(
                        validator, &point);
                while ((validator != nullptr) &&
                    (tobii_research_screen_based_calibration_validation_is_collecting_data(
                         validator) != 0))
                    std::this_thread::sleep_for(std::chrono::milliseconds{100});
            }

            class Result;

            auto result() -> Result { return Result{validator}; }

            ~Enter() {
                if (validator != nullptr)
                    tobii_research_screen_based_calibration_validation_leave_validation_mode(
                        validator);
            }

            class Result {
              public:
                explicit Result(CalibrationValidator *validator) {
                    if (validator != nullptr)
                        tobii_research_screen_based_calibration_validation_compute(
                            validator, &result);
                }

                ~Result() {
                    tobii_research_screen_based_calibration_validation_destroy_result(
                        result);
                }

              private:
                CalibrationValidationResult *result{};
            };

          private:
            CalibrationValidator *validator{};
        };

      private:
        CalibrationValidator *validator{};
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

static void setAnimationEndFrame(
    NSMutableDictionary *mutableDictionary, float x, float y, double size) {
    const auto subjectScreen{[[NSScreen screens] lastObject]};
    const auto subjectScreenFrame{subjectScreen.frame};
    const auto subjectScreenSize{subjectScreenFrame.size};
    [mutableDictionary
        setObject:[NSValue
                      valueWithRect:NSMakeRect(
                                        x * subjectScreenSize.width - size / 2,
                                        (1 - y) * subjectScreenSize.height -
                                            size / 2,
                                        size, size)]
           forKey:NSViewAnimationEndFrameKey];
}

static void animate(NSViewAnimation *viewAnimation,
    NSMutableDictionary *mutableDictionary, float x, float y, double size,
    double durationSeconds) {
    const id lastFrame{
        [mutableDictionary valueForKey:NSViewAnimationEndFrameKey]};
    [mutableDictionary setObject:lastFrame forKey:NSViewAnimationStartFrameKey];
    setAnimationEndFrame(mutableDictionary, x, y, size);
    [viewAnimation setDuration:durationSeconds];
    [viewAnimation startAnimation];
}

static void main() {
    TobiiEyeTracker eyeTracker;
    AppKitTestSetupUIFactoryImpl testSetupViewFactory;
    DefaultOutputFileNameFactory outputFileNameFactory;
    const auto aboutViewController{
        [[ResizesToContentsViewController alloc] init]};
    const auto stack {
        [NSStackView stackViewWithViews:@[
            [NSImageView
                imageViewWithImage:[NSImage imageNamed:@"tobii-pro-logo.jpg"]],
            [NSTextField
                labelWithString:@"This application is powered by Tobii Pro"]
        ]]
    };
    stack.orientation = NSUserInterfaceLayoutOrientationVertical;
    addAutolayoutEnabledSubview(aboutViewController.view, stack);
    [NSLayoutConstraint activateConstraints:@[
        [stack.topAnchor
            constraintEqualToAnchor:aboutViewController.view.topAnchor
                           constant:8],
        [stack.bottomAnchor
            constraintEqualToAnchor:aboutViewController.view.bottomAnchor
                           constant:-8],
        [stack.leadingAnchor
            constraintEqualToAnchor:aboutViewController.view.leadingAnchor
                           constant:8],
        [stack.trailingAnchor
            constraintEqualToAnchor:aboutViewController.view.trailingAnchor
                           constant:-8]
    ]];

    const auto calibrationViewController{
        av_speech_in_noise::nsTabViewControllerWithoutTabControl()};
    const auto subjectScreen{[[NSScreen screens] lastObject]};
    const auto subjectScreenFrame{subjectScreen.frame};
    calibrationViewController.view.frame = subjectScreenFrame;
    const auto animatingWindow{
        [NSWindow windowWithContentViewController:calibrationViewController]};
    [animatingWindow setStyleMask:NSWindowStyleMaskBorderless];
    [animatingWindow setFrame:subjectScreenFrame display:YES];
    eye_tracker_calibration::AppKitView eyeTrackerCalibrationView{
        animatingWindow};
    eye_tracker_calibration::Presenter eyeTrackerCalibrationPresenter{
        eyeTrackerCalibrationView};
    auto calibrator{eyeTracker.calibration()};
    eye_tracker_calibration::Interactor interactor{
        eyeTrackerCalibrationPresenter, calibrator,
        {{0.5, 0.5}, {0.1F, 0.1F}, {0.1F, 0.9F}, {0.9F, 0.1F}, {0.9F, 0.9F}}};
    [animatingWindow makeKeyAndOrderFront:nil];
    interactor.calibrate();

    initializeAppAndRunEventLoop(eyeTracker, testSetupViewFactory,
        outputFileNameFactory, aboutViewController);
}
}

int main() {
    const auto subjectScreen{[[NSScreen screens] lastObject]};
    const auto subjectScreenFrame{subjectScreen.frame};
    const auto subjectScreenOrigin{subjectScreenFrame.origin};
    const auto subjectScreenSize{subjectScreenFrame.size};
    const auto subjectScreenWidth{subjectScreenSize.width};
    const auto alertWindow{[[NSWindow alloc]
        initWithContentRect:NSMakeRect(
                                subjectScreenOrigin.x + subjectScreenWidth / 4,
                                subjectScreenOrigin.y +
                                    subjectScreenSize.height / 12,
                                subjectScreenWidth / 2,
                                subjectScreenSize.height / 2)
                  styleMask:NSWindowStyleMaskBorderless
                    backing:NSBackingStoreBuffered
                      defer:YES]};
    const auto alert{[[NSAlert alloc] init]};
    [alert setMessageText:@""];
    [alert
        setInformativeText:
            @"This software will store your eye tracking data.\n\nWe do so "
            @"only for the purpose of the current study (17-13-XP). We never "
            @"sell, distribute, or make profit on the collected data. Only "
            @"staff and research personnel on the existing IRB will have "
            @"access to the data. Any data used for publication or "
            @"collaborative and/or learning purposes will be "
            @"deidentified.\n\nThere is no direct benefit to you for doing "
            @"this study. What we learn from this study may help doctors treat "
            @"children who have a hard time hearing when it is noisy."];
    [alert addButtonWithTitle:@"No, I do not accept"];
    [alert addButtonWithTitle:@"Yes, I accept"];
    [alertWindow makeKeyAndOrderFront:nil];
    [alert beginSheetModalForWindow:alertWindow
                  completionHandler:^(NSModalResponse returnCode) {
                    [alertWindow orderOut:nil];
                    [NSApp stopModalWithCode:returnCode];
                  }];
    if ([NSApp runModalForWindow:alertWindow] == NSAlertFirstButtonReturn) {
        const auto terminatingAlert{[[NSAlert alloc] init]};
        [terminatingAlert setMessageText:@""];
        [terminatingAlert setInformativeText:@"User does not accept eye "
                                             @"tracking terms. Terminating."];
        [terminatingAlert runModal];
    } else
        av_speech_in_noise::main();
}
