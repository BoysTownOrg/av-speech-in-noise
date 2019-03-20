#include "StimulusPlayerImpl.hpp"

namespace stimulus_player {
    StimulusPlayerImpl::StimulusPlayerImpl(VideoPlayer *player) :
        player{player} {}

    void StimulusPlayerImpl::subscribe(EventListener *) {
    
    }

    void StimulusPlayerImpl::setDevice(int index) {
        player->setDevice(index);
    }

    void StimulusPlayerImpl::play() {
        player->play();
    }

    void StimulusPlayerImpl::loadFile(std::string filePath) {
        player->loadFile(filePath);
    }

    void StimulusPlayerImpl::hideVideo() {
        player->hide();
    }

    void StimulusPlayerImpl::showVideo() {
        player->show();
    }

    double StimulusPlayerImpl::rms() {
        return 0;
    }

    void StimulusPlayerImpl::setLevel_dB(double) {
    
    }
}
