#ifndef av_coordinate_response_measure_OutputFileImpl_hpp
#define av_coordinate_response_measure_OutputFileImpl_hpp

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
        virtual std::string generateFileName(const Test &) = 0;
        virtual std::string homeDirectory() = 0;
        virtual std::string outputDirectory() = 0;
    };

    class OutputFileImpl : public OutputFile {
        Writer *writer;
        OutputFilePath *path;
    public:
        OutputFileImpl(Writer *, OutputFilePath *);
        void writeTest(const Test &) override;
        void writeTrial(const Trial &) override;
        void writeTrialHeading() override;
        void openNewFile(const Test &) override;
        void close() override;
        
    private:
        void write(std::string);
        std::string evaluation(const Trial &);
        std::string formatTest(const Test &);
        std::string formatTrial(const Trial &);
        std::string formatTrialHeading();
        std::string generateNewFilePath(const Test &);
        std::string colorName(Color);
    };
}


#endif
