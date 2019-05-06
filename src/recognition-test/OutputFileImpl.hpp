#ifndef av_speech_in_noise_OutputFileImpl_hpp
#define av_speech_in_noise_OutputFileImpl_hpp

#include "RecognitionTestModel.hpp"

namespace av_speech_in_noise {
    class Writer {
    public:
        virtual ~Writer() = default;
        virtual void write(std::string) = 0;
        virtual void open(std::string) = 0;
        virtual bool failed() = 0;
        virtual void close() = 0;
    };
    
    class OutputFilePath {
    public:
        virtual ~OutputFilePath() = default;
        virtual std::string generateFileName(const TestInformation &) = 0;
        virtual std::string homeDirectory() = 0;
        virtual std::string outputDirectory() = 0;
    };

    class OutputFileImpl : public OutputFile {
        Writer *writer;
        OutputFilePath *path;
    public:
        OutputFileImpl(Writer *, OutputFilePath *);
        void writeTest(const AdaptiveTest &) override;
        void writeTrial(const coordinate_response_measure::Trial &) override;
        void writeCoordinateResponseTrialHeading() override;
        void openNewFile(const TestInformation &) override;
        void close() override;
        
    private:
        void write(std::string);
        std::string evaluation(const coordinate_response_measure::Trial &);
        std::string formatTest(const AdaptiveTest &);
        std::string formatTrial(const coordinate_response_measure::Trial &);
        std::string formatCoordinateResponseTrialHeading();
        std::string generateNewFilePath(const TestInformation &);
        std::string colorName(coordinate_response_measure::Color);
    };
}


#endif
