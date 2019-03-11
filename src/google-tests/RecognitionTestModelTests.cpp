#include <recognition-test/Model.hpp>
#include <gtest/gtest.h>

class MaskerPlayerStub : public recognition_test::MaskerPlayer {
    std::string filePath_{};
    EventListener *listener_{};
    bool fadeInCalled_{};
    bool fadeOutCalled_{};
public:
    bool fadeInCalled() const {
        return fadeInCalled_;
    }
    
    void fadeIn() override {
        fadeInCalled_ = true;
    }
    bool fadeOutCalled() const {
        return fadeOutCalled_;
    }
    
    void subscribe(EventListener *listener) override {
        listener_ = listener;
    }
    
    void fadeOut() override {
        fadeOutCalled_ = true;
    }
    
    void loadFile(std::string filePath) override {
        filePath_ = filePath;
    }
    
    const EventListener *listener() const {
        return listener_;
    }
    
    void fadeInComplete() {
        listener_->fadeInComplete();
    }
    
    std::string filePath() const {
        return filePath_;
    }
};

class StimulusPlayerStub : public recognition_test::StimulusPlayer {
    std::string filePath_{};
    EventListener *listener_{};
    bool played_{};
public:
    bool played() const {
        return played_;
    }
    
    void play() override {
        played_ = true;
    }
    
    void subscribe(EventListener *listener) override {
        listener_ = listener;
    }
    
    void loadFile(std::string filePath) override {
        filePath_ = filePath;
    }
    
    void playbackComplete() {
        listener_->playbackComplete();
    }
    
    const EventListener *listener() const {
        return listener_;
    }
    
    std::string filePath() const {
        return filePath_;
    }
};

class SubjectViewStub : public recognition_test::SubjectView {
    StimulusPlayerStub *player;
public:
    explicit SubjectViewStub(StimulusPlayerStub *player) : player{player} {}
    
    recognition_test::StimulusPlayer *stimulusPlayer() override {
        return player;
    }
};

class StimulusListStub : public recognition_test::StimulusList {
    std::string directory_{};
    std::string next_{};
    bool empty_{};
public:
    std::string directory() const {
        return directory_;
    }
    
    void initialize(std::string directory) override {
        directory_ = directory;
    }
    
    bool empty() override {
        return empty_;
    }
    
    void setEmpty() {
        empty_ = true;
    }
    
    std::string next() override {
        return next_;
    }
    
    void setNext(std::string s) {
        next_ = std::move(s);
    }
};

class RecognitionTestModelTests : public ::testing::Test {
protected:
    recognition_test::Model::TestParameters testParameters;
    StimulusPlayerStub stimulusPlayer{};
    SubjectViewStub view{&stimulusPlayer};
    MaskerPlayerStub maskerPlayer{};
    StimulusListStub list{};
    recognition_test::Model model{&maskerPlayer, &list, &view};
    
    void initializeTest() {
        model.initializeTest(testParameters);
    }
};

TEST_F(RecognitionTestModelTests, subscribesToPlayerEvents) {
    EXPECT_EQ(&model, stimulusPlayer.listener());
    EXPECT_EQ(&model, maskerPlayer.listener());
}

TEST_F(RecognitionTestModelTests, playTrialFadesInMasker) {
    model.playTrial({});
    EXPECT_TRUE(maskerPlayer.fadeInCalled());
}

TEST_F(RecognitionTestModelTests, fadeInCompletePlaysStimulus) {
    maskerPlayer.fadeInComplete();
    EXPECT_TRUE(stimulusPlayer.played());
}

TEST_F(RecognitionTestModelTests, stimulusPlaybackCompleteFadesOutMasker) {
    stimulusPlayer.playbackComplete();
    EXPECT_TRUE(maskerPlayer.fadeOutCalled());
}

static void assertEqual(std::string expected, std::string actual) {
    EXPECT_EQ(expected, actual);
}

TEST_F(
    RecognitionTestModelTests,
    initializeTestPassesMaskerFilePathToMaskerPlayer
) {
    testParameters.maskerFilePath = "a";
    initializeTest();
    assertEqual("a", maskerPlayer.filePath());
}

TEST_F(
    RecognitionTestModelTests,
    initializeTestPassesStimulusListDirectoryToStimulusList
) {
    testParameters.stimulusListDirectory = "a";
    initializeTest();
    assertEqual("a", list.directory());
}

TEST_F(
    RecognitionTestModelTests,
    playTrialPassesNextStimulusToStimulusPlayer
) {
    list.setNext("a");
    model.playTrial({});
    assertEqual("a", stimulusPlayer.filePath());
}

TEST_F(
    RecognitionTestModelTests,
    testCompleteWhenListEmpty
) {
    list.setEmpty();
    EXPECT_TRUE(model.testComplete());
}

TEST_F(
    RecognitionTestModelTests,
    playTrialDoesNotPlayIfListEmpty
) {
    list.setEmpty();
    model.playTrial({});
    EXPECT_FALSE(maskerPlayer.fadeInCalled());
}
