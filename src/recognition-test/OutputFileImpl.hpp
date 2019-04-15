#ifndef av_coordinated_response_measure_OutputFileImpl_hpp
#define av_coordinated_response_measure_OutputFileImpl_hpp

#include "RecognitionTestModel.hpp"

namespace av_coordinated_response_measure {
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
        virtual std::string generateFileName(
            const av_coordinated_response_measure::Test &
        ) = 0;
        virtual std::string homeDirectory() = 0;
        virtual std::string outputDirectory() = 0;
    };

    class OutputFileImpl : public OutputFile {
        Writer *writer;
        OutputFilePath *path;
    public:
        OutputFileImpl(Writer *, OutputFilePath *);
        void writeTest(const av_coordinated_response_measure::Test &) override;
        void writeTrial(const av_coordinated_response_measure::Trial &) override;
        void writeTrialHeading() override;
        void openNewFile(const av_coordinated_response_measure::Test &) override;
        void close() override;
        
    private:
        void write(std::string);
        std::string evaluation(const av_coordinated_response_measure::Trial &);
        std::string formatTest(const av_coordinated_response_measure::Test &);
        std::string formatTrial(const av_coordinated_response_measure::Trial &);
        std::string formatTrialHeading();
        std::string generateNewFilePath(const av_coordinated_response_measure::Test &);
        std::string colorName(av_coordinated_response_measure::Color);
    };
}


#endif
