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
    }
    
    enum class TrackProperty {
        up,
        down,
        reversalsPerStepSize,
        stepSizes
    };

    constexpr const char *propertyName(TrackProperty p) {
        switch (p) {
            case TrackProperty::up:
                return "up";
            case TrackProperty::down:
                return "down";
            case TrackProperty::reversalsPerStepSize:
                return "reversals per step size";
            case TrackProperty::stepSizes:
                return "step sizes (dB)";
        }
    }
    
    class TrackSettingsReader {
        std::map<std::string, void(TrackSettingsReader::*)(TrackingSequence &, int)> propertyApplication {
            {propertyName(TrackProperty::up), &TrackSettingsReader::applyToUp},
            {propertyName(TrackProperty::down), &TrackSettingsReader::applyToDown},
            {propertyName(TrackProperty::reversalsPerStepSize), &TrackSettingsReader::applyToRunCount},
            {propertyName(TrackProperty::stepSizes), &TrackSettingsReader::applyToStepSize}
        };
        std::string contents;
    public:
        
        explicit TrackSettingsReader(std::string s) :
            contents{std::move(s)} {}
        
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
            for (int i = 0; i < 4; ++i) {
                auto f = propertyApplication.at(stream_.propertyName());
                for (auto &sequence : rule)
                    (this->*f)(sequence, stream_.value());
                stream_.nextLine();
            }
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
            std::vector<std::string> v,
            const TrackingRule &expected
        ) {
            TrackSettingsReader reader{concatenate(std::move(v))};
            assertEqual(expected, reader.trackingRule());
        }
        
        std::string concatenate(std::vector<std::string> v) {
            std::string result;
            for (auto v_ : std::move(v))
                result.append(v_);
            return result;
        }
        
        std::string propertyEntry(TrackProperty p, std::string s) {
            return std::string{propertyName(p)} + ": " + std::move(s);
        }
        
        std::string propertyEntryWithNewline(TrackProperty p, std::string s) {
            return propertyEntry(p, std::move(s)) + '\n';
        }
    };
    
    TEST_F(TrackSettingsReaderTests, oneSequence) {
        TrackingSequence first;
        first.up = 1;
        first.down = 2;
        first.runCount = 3;
        first.stepSize = 4;
        assertFileContentsYield(
            {
                propertyEntryWithNewline(TrackProperty::up, "1"),
                propertyEntryWithNewline(TrackProperty::down, "2"),
                propertyEntryWithNewline(TrackProperty::reversalsPerStepSize, "3"),
                propertyEntry(TrackProperty::stepSizes, "4")
            },
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
            {
                propertyEntryWithNewline(TrackProperty::up, "1 2"),
                propertyEntryWithNewline(TrackProperty::down, "3 4"),
                propertyEntryWithNewline(TrackProperty::reversalsPerStepSize, "5 6"),
                propertyEntry(TrackProperty::stepSizes, "7 8")
            },
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
            {
                propertyEntryWithNewline(TrackProperty::down, "2"),
                propertyEntryWithNewline(TrackProperty::up, "1"),
                propertyEntryWithNewline(TrackProperty::reversalsPerStepSize, "3"),
                propertyEntry(TrackProperty::stepSizes, "4")
            },
            {first}
        );
    }
}
