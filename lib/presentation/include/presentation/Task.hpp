#ifndef AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_TASK_HPP_
#define AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_TASK_HPP_

namespace av_speech_in_noise {
class ParentPresenter {
  public:
    virtual ~ParentPresenter() = default;
    virtual void playTrial() = 0;
    virtual void playNextTrialIfNeeded() = 0;
    virtual void readyNextTrialIfNeeded() = 0;
    virtual void showContinueTestingDialogWithResultsWhenComplete() = 0;
};

class TaskResponder {
  public:
    class EventListener {
      public:
        virtual ~EventListener() = default;
        virtual void notifyThatTaskHasStarted() = 0;
        virtual void notifyThatUserIsDoneResponding() = 0;
    };
    virtual ~TaskResponder() = default;
    virtual void becomeChild(ParentPresenter *) = 0;
    virtual void subscribe(EventListener *) = 0;
};

class TaskPresenter : virtual public TaskResponder::EventListener {
  public:
    virtual void start() = 0;
    virtual void stop() = 0;
    virtual void showResponseSubmission() = 0;
};
}

#endif