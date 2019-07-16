#include "TrackSettingsReader.hpp"
#include <sstream>

namespace av_speech_in_noise {
    namespace {
        class Stream {
            std::stringstream parent;
            std::stringstream line_{};
            std::string lastLine_{};
            bool failed_{};
        public:
            explicit Stream(std::string s) : parent{std::move(s)} {}
            
            void nextLine() {
                std::getline(parent, lastLine_);
                resetLine_();
            }
            
            void resetLine() {
                resetLine_();
            }
            
            std::string propertyName() {
                return lastLine_.substr(0, findPropertyNameDelimiter());
            }
            
            void resetLine_() {
                line_ = std::stringstream{lastLine_.substr(findPropertyNameDelimiter()+1)};
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
        
        void(*propertyApplication(const std::string &s))(TrackingSequence &, int) {
            using Property = TrackSettingsReader::Property;
            if (s == TrackSettingsReader::propertyName(Property::up))
                return applyToUp;
            if (s == TrackSettingsReader::propertyName(Property::down))
                return applyToDown;
            if (s == TrackSettingsReader::propertyName(Property::reversalsPerStepSize))
                return applyToRunCount;
            if (s == TrackSettingsReader::propertyName(Property::stepSizes))
                return applyToStepSize;
            return {};
        }
    }
    
    TrackSettingsReader::TrackSettingsReader(std::string s) :
        contents{std::move(s)} {}
    
    TrackingRule TrackSettingsReader::trackingRule() {
        auto stream_ = Stream{contents};
        stream_.nextLine();
        TrackingRule rule;
        stream_.value();
        while (!stream_.failed()) {
            rule.push_back({});
            stream_.value();
        }
        stream_.resetLine();
        for (int i = 0; i < 4; ++i) {
            auto f = propertyApplication(stream_.propertyName());
            for (auto &sequence : rule)
                (*f)(sequence, stream_.value());
            stream_.nextLine();
        }
        return rule;
    }
}
