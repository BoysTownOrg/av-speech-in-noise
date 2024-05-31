#include "Timer.h"

#import <Foundation/Foundation.h>

@interface CallbackScheduler : NSObject
@end

@implementation CallbackScheduler {
  @public
    av_speech_in_noise::TimerImpl *controller;
    NSTimer *lastTimer;
}

- (void)scheduleCallbackAfterSeconds:(double)x {
    lastTimer = [NSTimer scheduledTimerWithTimeInterval:x
                                                 target:self
                                               selector:@selector(timerCallback)
                                               userInfo:nil
                                                repeats:NO];
}

- (void)timerCallback {
    controller->timerCallback();
}
@end

namespace av_speech_in_noise {
TimerImpl::TimerImpl() : scheduler{[[CallbackScheduler alloc] init]} {
    scheduler->controller = this;
}

void TimerImpl::attach(Observer *e) { listener = e; }

void TimerImpl::scheduleCallbackAfterSeconds(double x) {
    [scheduler scheduleCallbackAfterSeconds:x];
}

void TimerImpl::timerCallback() { listener->callback(); }

void TimerImpl::cancelLastCallback() { [scheduler->lastTimer invalidate]; }
}
