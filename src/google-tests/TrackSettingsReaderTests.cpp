#include <av-speech-in-noise/Model.h>
#include <string>
#include <sstream>
#include <map>

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
                auto found = lastLine_.find(':');
                auto s = lastLine_.substr(0, found);
                return s;
            }
            
            void resetLine_() {
                auto found = lastLine_.find(':');
                line_ = std::stringstream{lastLine_.substr(found+1)};
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
    }
    
    class TrackSettingsReader {
        std::map<std::string, void(TrackSettingsReader::*)(TrackingSequence &, int)> propertyApplication {
            {"up", &TrackSettingsReader::applyToUp},
            {"down", &TrackSettingsReader::applyToDown},
            {"reversals per step size", &TrackSettingsReader::applyToRunCount},
            {"step sizes (dB)", &TrackSettingsReader::applyToStepSize}
        };
        std::string contents;
    public:
        explicit TrackSettingsReader(std::string s) : contents{std::move(s)} {}
        
        TrackingRule trackingRule() {
            auto stream_ = Stream{contents};
            stream_.nextLine();
            TrackingRule rule;
            stream_.value();
            while (!stream_.failed()) {
                rule.push_back({});
                stream_.value();
            }
            stream_.resetLine();
            auto f = propertyApplication.at(stream_.propertyName());
            for (auto &sequence : rule)
                (this->*f)(sequence, stream_.value());
            stream_.nextLine();
            f = propertyApplication.at(stream_.propertyName());
            for (auto &sequence : rule)
                (this->*f)(sequence, stream_.value());
            stream_.nextLine();
            for (auto &sequence : rule)
                sequence.runCount = stream_.value();
            stream_.nextLine();
            for (auto &sequence : rule)
                sequence.stepSize = stream_.value();
            return rule;
        }
        
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
    };
}

#include "assert-utility.h"
#include <gtest/gtest.h>

namespace av_speech_in_noise::tests {
    class TrackSettingsReaderTests : public ::testing::Test {
    protected:
        void assertFileContentsYield(
            std::string s,
            const TrackingRule &expected
        ) {
            TrackSettingsReader reader{std::move(s)};
            assertEqual(expected, reader.trackingRule());
        }
    };
    
    TEST_F(TrackSettingsReaderTests, oneSequence) {
        TrackingSequence first;
        first.up = 1;
        first.down = 2;
        first.runCount = 3;
        first.stepSize = 4;
        assertFileContentsYield(
            "up: 1\n"
            "down: 2\n"
            "reversals per step size: 3\n"
            "step sizes (dB): 4",
            {first}
        );
    }
    
    TEST_F(TrackSettingsReaderTests, twoSequences) {
        TrackingSequence first;
        first.up = 1;
        first.down = 3;
        first.runCount = 5;
        first.stepSize = 7;
        TrackingSequence second;
        second.up = 2;
        second.down = 4;
        second.runCount = 6;
        second.stepSize = 8;
        assertFileContentsYield(
            "up: 1 2\n"
            "down: 3 4\n"
            "reversals per step size: 5 6\n"
            "step sizes (dB): 7 8",
            {first, second}
        );
    }
    
    TEST_F(TrackSettingsReaderTests, differentPropertyOrder) {
        TrackingSequence first;
        first.up = 1;
        first.down = 2;
        first.runCount = 3;
        first.stepSize = 4;
        assertFileContentsYield(
            "down: 2\n"
            "up: 1\n"
            "reversals per step size: 3\n"
            "step sizes (dB): 4",
            {first}
        );
    }
}
