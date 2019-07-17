#include "TrackSettingsInterpreter.hpp"
#include <sstream>

namespace av_speech_in_noise {
    namespace {
        class Stream {
            std::stringstream stream;
            std::stringstream line_{};
            std::string lastLine_{};
            bool failed_{};
        public:
            explicit Stream(std::string s) : stream{std::move(s)} {}
            
            bool nextLine() {
                if (!std::getline(stream, lastLine_))
                    return false;
                resetLine_();
                return true;
            }
            
            std::string propertyName() {
                return lastLine_.substr(0, findPropertyNameDelimiter());
            }
            
            void resetLine_() {
                line_ = std::stringstream{
                    lastLine_.substr(findPropertyNameDelimiter()+1)
                };
                failed_ = false;
            }
            
            std::string::size_type findPropertyNameDelimiter() {
                return lastLine_.find(':');
            }
            
            int value() {
                int x;
                if (!(line_ >> x))
                    failed_ = true;
                return x;
            }
            
            auto failed() const {
                return failed_;
            }
        };
    
        void applyToUp(TrackingSequence &sequence, int x) {
            sequence.up = x;
        }

        void applyToDown(TrackingSequence &sequence, int x) {
            sequence.down = x;
        }

        void applyToRunCount(TrackingSequence &sequence, int x) {
            sequence.runCount = x;
        }

        void applyToStepSize(TrackingSequence &sequence, int x) {
            sequence.stepSize = x;
        }

        void nothing(TrackingSequence &, int) {}
        
        void(*propertyApplication(const std::string &s))(TrackingSequence &, int) {
            using Property = TrackSettingsInterpreter::Property;
            if (s == TrackSettingsInterpreter::propertyName(Property::up))
                return applyToUp;
            if (s == TrackSettingsInterpreter::propertyName(Property::down))
                return applyToDown;
            if (s == TrackSettingsInterpreter::propertyName(Property::reversalsPerStepSize))
                return applyToRunCount;
            if (s == TrackSettingsInterpreter::propertyName(Property::stepSizes))
                return applyToStepSize;
            return nothing;
        }
    }
    
    TrackSettingsInterpreter::TrackSettingsInterpreter(std::string s) :
        contents{std::move(s)} {}
    
    TrackingRule TrackSettingsInterpreter::trackingRule() {
        auto stream = Stream{contents};
        TrackingRule rule;
        while (stream.nextLine()) {
            auto sequenceCount{0U};
            auto f = propertyApplication(stream.propertyName());
            while (true) {
                auto value = stream.value();
                if (stream.failed())
                    break;
                if (sequenceCount == rule.size())
                    rule.push_back({});
                auto &sequence = rule.at(sequenceCount++);
                (*f)(sequence, value);
            }
        }
        return rule;
    }
    
    const TrackingRule *TrackSettingsInterpreter::trackingRule(std::string s) {
        auto stream = Stream{std::move(s)};
        while (stream.nextLine()) {
            auto sequenceCount{0U};
            auto f = propertyApplication(stream.propertyName());
            while (true) {
                auto value = stream.value();
                if (stream.failed())
                    break;
                if (sequenceCount == rule_.size())
                    rule_.push_back({});
                auto &sequence = rule_.at(sequenceCount++);
                (*f)(sequence, value);
            }
        }
        return &rule_;
    }
}
