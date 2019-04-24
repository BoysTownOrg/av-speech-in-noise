#include "MaskerPlayerImpl.hpp"
#include <cmath>

namespace stimulus_players {
    MaskerPlayerImpl::MaskerPlayerImpl(
        AudioPlayer *player,
        AudioReader *reader,
        Timer *timer
    ) :
        audioThread{player},
        mainThread{player, timer},
        player{player},
        reader{reader}
    {
        player->subscribe(this);
        timer->subscribe(this);
        mainThread.setSharedAtomics(this);
        audioThread.setSharedAtomics(this);
    }
    
    MaskerPlayerImpl::MainThread::MainThread(
        AudioPlayer *player,
        Timer *timer
    ) :
        player{player},
        timer{timer} {}
    
    MaskerPlayerImpl::AudioThread::AudioThread(AudioPlayer *player) :
        player{player}
    {}

    void MaskerPlayerImpl::MainThread::setSharedAtomics(MaskerPlayerImpl *p) {
        sharedAtomics = p;
    }
    
    void MaskerPlayerImpl::MainThread::subscribe(MaskerPlayer::EventListener *e) {
        listener = e;
    }

    void MaskerPlayerImpl::AudioThread::setSharedAtomics(MaskerPlayerImpl *p) {
        sharedAtomics = p;
    }

    void MaskerPlayerImpl::subscribe(MaskerPlayer::EventListener *e) {
        mainThread.subscribe(e);
    }
    
    double MaskerPlayerImpl::durationSeconds() {
        return player->durationSeconds();
    }
    
    void MaskerPlayerImpl::seekSeconds(double x) {
        player->seekSeconds(x);
    }
    
    double MaskerPlayerImpl::fadeTimeSeconds() {
        return fadeInOutSeconds.load();
    }
    
    void MaskerPlayerImpl::fadeIn() {
        mainThread.fadeIn();
    }
    
    void MaskerPlayerImpl::MainThread::fadeIn() {
        if (fading())
            return;
        
        fadingIn = true;
        sharedAtomics->pleaseFadeIn.store(true);
        player->play();
        timer->scheduleCallbackAfterSeconds(0.1);
    }
    
    bool MaskerPlayerImpl::MainThread::fading() {
        return fadingIn || fadingOut;
    }

    void MaskerPlayerImpl::fadeOut() {
        mainThread.fadeOut();
    }

    void MaskerPlayerImpl::MainThread::fadeOut() {
        if (fading())
            return;
        
        fadingOut = true;
        sharedAtomics->pleaseFadeOut.store(true);
        timer->scheduleCallbackAfterSeconds(0.1);
    }

    void MaskerPlayerImpl::loadFile(std::string filePath) {
        player->loadFile(filePath_ = std::move(filePath));
    }

    bool MaskerPlayerImpl::playing() {
        return player->playing();
    }

    void MaskerPlayerImpl::setLevel_dB(double x) {
        levelScalar.store(std::pow(10, x/20));
    }
    
    void MaskerPlayerImpl::setFadeInOutSeconds(double x) {
        fadeInOutSeconds.store(x);
    }
    
    void MaskerPlayerImpl::setAudioDevice(std::string device) {
        player->setDevice(findDeviceIndex(device));
    }
    
    int MaskerPlayerImpl::findDeviceIndex(const std::string &device) {
        auto devices_ = audioDeviceDescriptions_();
        auto found = std::find(
            devices_.begin(),
            devices_.end(),
            device
        );
        if (found == devices_.end())
            throw av_coordinate_response_measure::InvalidAudioDevice{};
        return gsl::narrow<int>(found - devices_.begin());
    }
    
    template<typename T>
    T rms(const std::vector<T> &x) {
        return std::sqrt(
            std::accumulate(
                x.begin(),
                x.end(),
                T{ 0 },
                [](T a, T b) { return a += b * b; }
            ) / x.size()
        );
    }

    double MaskerPlayerImpl::rms() {
        auto audio = readAudio_();
        if (audio.empty())
            return 0;
        
        auto firstChannel = audio.front();
        return ::stimulus_players::rms(firstChannel);
    }

    std::vector<std::vector<float>> MaskerPlayerImpl::readAudio_() {
        try {
            return reader->read(filePath_);
        } catch (const AudioReader::InvalidFile &) {
            throw av_coordinate_response_measure::InvalidAudioFile{};
        }
    }
    
    std::vector<std::string> MaskerPlayerImpl::audioDeviceDescriptions_() {
        std::vector<std::string> descriptions{};
        for (int i = 0; i < player->deviceCount(); ++i)
            descriptions.push_back(player->deviceDescription(i));
        return descriptions;
    }
    
    std::vector<std::string> MaskerPlayerImpl::outputAudioDeviceDescriptions() {
        std::vector<std::string> descriptions{};
        for (int i = 0; i < player->deviceCount(); ++i)
            if (player->outputDevice(i))
                descriptions.push_back(player->deviceDescription(i));
        return descriptions;
    }
    
    void MaskerPlayerImpl::callback() {
        mainThread.callback();
    }

    void MaskerPlayerImpl::MainThread::callback() {
        auto expectedFadeInComplete = true;
        if (sharedAtomics->fadeInComplete.compare_exchange_strong(
            expectedFadeInComplete,
            false
        )) {
            fadingIn = false;
            listener->fadeInComplete();
            return;
        }
        
        auto expectedFadeOutComplete = true;
        if (sharedAtomics->fadeOutComplete.compare_exchange_strong(
            expectedFadeOutComplete,
            false
        )) {
            fadingOut = false;
            listener->fadeOutComplete();
            player->stop();
            return;
        }
        
        timer->scheduleCallbackAfterSeconds(0.1);
    }
    
    // real-time audio thread
    void MaskerPlayerImpl::fillAudioBuffer(
        const std::vector<gsl::span<float>> &audio
    ) {
        audioThread.fillAudioBuffer(audio);
    }

    void MaskerPlayerImpl::AudioThread::fillAudioBuffer(
        const std::vector<gsl::span<float>> &audio
    ) {
        checkForFadeIn();
        checkForFadeOut();
        scaleAudio(audio);
    }
    
    void MaskerPlayerImpl::AudioThread::checkForFadeIn() {
        auto expected = true;
        if (sharedAtomics->pleaseFadeIn.compare_exchange_strong(expected, false))
            prepareToFadeIn();
    }
    
    void MaskerPlayerImpl::AudioThread::prepareToFadeIn() {
        updateWindowLength();
        hannCounter = 0;
        fadingIn = true;
    }
    
    void MaskerPlayerImpl::AudioThread::updateWindowLength() {
        halfWindowLength = levelTransitionSamples();
    }
    
    void MaskerPlayerImpl::AudioThread::checkForFadeOut() {
        auto expected = true;
        if (sharedAtomics->pleaseFadeOut.compare_exchange_strong(expected, false))
            prepareToFadeOut();
    }
    
    void MaskerPlayerImpl::AudioThread::prepareToFadeOut() {
        updateWindowLength();
        hannCounter = halfWindowLength;
        fadingOut = true;
    }

    int MaskerPlayerImpl::AudioThread::levelTransitionSamples() {
        return sharedAtomics->fadeInOutSeconds.load() * player->sampleRateHz();
    }
    
    void MaskerPlayerImpl::AudioThread::scaleAudio(
        const std::vector<gsl::span<float>> &audio
    ) {
        if (audio.size() == 0)
            return;
        
        auto firstChannel = audio.front();
        auto levelScalar_ = sharedAtomics->levelScalar.load();
        for (int i = 0; i < firstChannel.size(); ++i) {
            auto fadeScalar_ = fadeScalar();
            updateFadeState();
            for (size_t channel = 0; channel < audio.size(); ++channel)
                audio.at(channel).at(i) *= fadeScalar_ * levelScalar_;
        }
    }
    
    static const auto pi = std::acos(-1);
    
    double MaskerPlayerImpl::AudioThread::fadeScalar() {
        const auto squareRoot = halfWindowLength
            ? std::sin((pi*hannCounter) / (2*halfWindowLength))
            : 1;
        return squareRoot * squareRoot;
    }
    
    void MaskerPlayerImpl::AudioThread::updateFadeState() {
        checkForFadeInComplete();
        checkForFadeOutComplete();
        advanceCounterIfStillFading();
    }
    
    void MaskerPlayerImpl::AudioThread::checkForFadeInComplete() {
        if (doneFadingIn()) {
            sharedAtomics->fadeInComplete.store(true);
            fadingIn = false;
        }
    }
    
    bool MaskerPlayerImpl::AudioThread::doneFadingIn() {
        return fadingIn && hannCounter == halfWindowLength;
    }
    
    bool MaskerPlayerImpl::AudioThread::doneFadingOut() {
        return fadingOut && hannCounter == 2*halfWindowLength;
    }
    
    void MaskerPlayerImpl::AudioThread::checkForFadeOutComplete() {
        if (doneFadingOut()) {
            sharedAtomics->fadeOutComplete.store(true);
            fadingOut = false;
        }
    }
    
    void MaskerPlayerImpl::AudioThread::advanceCounterIfStillFading() {
        if (fadingIn || fadingOut)
            ++hannCounter;
    }
}
