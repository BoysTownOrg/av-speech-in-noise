#include "../run.h"
#include "../AppKitView.h"
#include "../AppKit-utility.h"
#include <exception>
#include <tobii_research.h>
#include <tobii_research_streams.h>
#include <tobii_research_calibration.h>
#include <gsl/gsl>
#include <vector>
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

namespace av_speech_in_noise {
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

  private:
    class Calibration;

  public:
    auto calibration() -> Calibration {
        return Calibration{eyeTracker(eyeTrackers)};
    }

  private:
    class Calibration {
      public:
        Calibration(TobiiResearchEyeTracker *eyetracker) {
            tobii_research_screen_based_calibration_enter_calibration_mode(
                eyetracker);
        }

        void collect(float x, float y) {
            tobii_research_screen_based_calibration_collect_data(
                eyetracker, x, y);
        }

        void discard(float x, float y) {
            tobii_research_screen_based_calibration_discard_data(
                eyetracker, x, y);
        }

        auto successfullyComputesAndApplies() -> bool {
            ComputeAndApply computeAndApply{eyetracker};
            return computeAndApply.success();
        }

        ~Calibration() {
            tobii_research_screen_based_calibration_leave_calibration_mode(
                eyetracker);
        }

      private:
        TobiiResearchEyeTracker *eyetracker;

        class ComputeAndApply {
          public:
            ComputeAndApply(TobiiResearchEyeTracker *eyetracker) {
                tobii_research_screen_based_calibration_compute_and_apply(
                    eyetracker, &calibration_result);
            }

            auto success() -> bool {
                return calibration_result->status ==
                    TOBII_RESEARCH_CALIBRATION_SUCCESS;
            }

            ~ComputeAndApply() {
                tobii_research_free_screen_based_calibration_result(
                    calibration_result);
            }

          private:
            TobiiResearchCalibrationResult *calibration_result{};
        };
    };

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
    NSMutableDictionary *mutableDictionary, double x, double y, double size) {
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
    NSMutableDictionary *mutableDictionary, double x, double y, double size,
    double durationSeconds) {
    [mutableDictionary
        setObject:[mutableDictionary valueForKey:NSViewAnimationEndFrameKey]
           forKey:NSViewAnimationStartFrameKey];
    setAnimationEndFrame(mutableDictionary, x, y, size);
    [viewAnimation setDuration:durationSeconds];
    [viewAnimation startAnimation];
}

void main() {
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
    const auto subjectScreenSize{subjectScreenFrame.size};
    calibrationViewController.view.frame = subjectScreenFrame;
    const auto circleView{
        [[CircleView alloc] initWithFrame:NSMakeRect(0, 0, 100, 100)]};
    [calibrationViewController.view addSubview:circleView];
    const auto animatingWindow{
        [NSWindow windowWithContentViewController:calibrationViewController]};
    [animatingWindow setStyleMask:NSWindowStyleMaskBorderless];
    [animatingWindow setFrame:subjectScreenFrame display:YES];
    [animatingWindow makeKeyAndOrderFront:nil];
    const auto mutableDictionary {
        [NSMutableDictionary
            dictionaryWithSharedKeySet:[NSDictionary sharedKeySetForKeys:@[
                NSViewAnimationTargetKey, NSViewAnimationStartFrameKey,
                NSViewAnimationEndFrameKey
            ]]]
    };
    [mutableDictionary setObject:circleView forKey:NSViewAnimationTargetKey];
    [mutableDictionary setObject:[NSValue valueWithRect:[circleView frame]]
                          forKey:NSViewAnimationEndFrameKey];
    const auto viewAnimation{[[NSViewAnimation alloc]
        initWithViewAnimations:[NSArray
                                   arrayWithObjects:mutableDictionary, nil]]};
    [viewAnimation setAnimationCurve:NSAnimationEaseInOut];
    viewAnimation.animationBlockingMode = NSAnimationBlocking;
    auto calibration{eyeTracker.calibration()};
    animate(viewAnimation, mutableDictionary, 0.5, 0.5, 100, 1.5);
    animate(viewAnimation, mutableDictionary, 0.5, 0.5, 25, 0.5);
    calibration.collect(0.5, 0.5);
    animate(viewAnimation, mutableDictionary, 0.5, 0.5, 100, 1.5);
    animate(viewAnimation, mutableDictionary, 0.1, 0.1, 100, 1.5);
    animate(viewAnimation, mutableDictionary, 0.1, 0.1, 25, 0.5);
    calibration.collect(0.1, 0.1);
    animate(viewAnimation, mutableDictionary, 0.1, 0.1, 100, 1.5);
    animate(viewAnimation, mutableDictionary, 0.1, 0.9, 100, 1.5);
    animate(viewAnimation, mutableDictionary, 0.1, 0.9, 25, 0.5);
    calibration.collect(0.1, 0.9);
    animate(viewAnimation, mutableDictionary, 0.1, 0.9, 100, 1.5);
    animate(viewAnimation, mutableDictionary, 0.9, 0.1, 100, 1.5);
    animate(viewAnimation, mutableDictionary, 0.9, 0.1, 25, 0.5);
    calibration.collect(0.9, 0.1);
    animate(viewAnimation, mutableDictionary, 0.9, 0.1, 100, 1.5);
    animate(viewAnimation, mutableDictionary, 0.9, 0.9, 100, 1.5);
    animate(viewAnimation, mutableDictionary, 0.9, 0.9, 25, 0.5);
    calibration.collect(0.9, 0.9);
    calibration.successfullyComputesAndApplies();

    initializeAppAndRunEventLoop(eyeTracker, testSetupViewFactory,
        outputFileNameFactory, aboutViewController);
}
}

int main() {
    const auto subjectScreen{[[NSScreen screens] lastObject]};
    const auto subjectScreenFrame{subjectScreen.frame};
    const auto subjectScreenOrigin{subjectScreenFrame.origin};
    const auto subjectScreenSize{subjectScreenFrame.size};
    const auto subjectViewHeight{subjectScreenSize.height / 4};
    const auto subjectScreenWidth{subjectScreenSize.width};
    const auto subjectViewWidth{subjectScreenWidth / 3};
    auto subjectViewLeadingEdge =
        subjectScreenOrigin.x + (subjectScreenWidth - subjectViewWidth) / 2;
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
