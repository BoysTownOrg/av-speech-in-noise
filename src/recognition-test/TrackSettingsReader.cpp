#include "TrackSettingsReader.hpp"

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
            auto f = propertyApplication.at(stream_.propertyName());
            for (auto &sequence : rule)
                (this->*f)(sequence, stream_.value());
            stream_.nextLine();
        }
        return rule;
    }
}
