#include <recognition-test/OutputFile.hpp>
#include <string>

namespace av_speech_in_noise {
struct TestSetupState {
    std::string masker;
};

enum class TestSetupStateItem { masker };

constexpr auto name(TestSetupStateItem p) -> const char * {
    switch (p) {
    case TestSetupStateItem::masker:
        return "masker";
    }
}

class TestSetupStateWriter {
    Writer &writer;

  public:
    explicit TestSetupStateWriter(Writer &writer) : writer{writer} {}

    void write(const TestSetupState &state) {
        using std::string_literals::operator""s;
        writer.write(
            name(TestSetupStateItem::masker) + ": "s + state.masker + "\n");
    }
};
}

#include "assert-utility.h"
#include "LogString.h"
#include <gtest/gtest.h>

namespace av_speech_in_noise::tests {
namespace {
class WriterStub : public Writer {
    LogString written_;
    std::string filePath_;
    bool closed_{};
    bool saved_{};

  public:
    void save() override { saved_ = true; }

    auto saved() const { return saved_; }

    void close() override { closed_ = true; }

    void open(std::string f) override { filePath_ = std::move(f); }

    auto filePath() const { return filePath_; }

    auto closed() const { return closed_; }

    auto written() const -> auto & { return written_; }

    void write(std::string s) override { written_.insert(s); }

    auto failed() -> bool override { return {}; }
};

class TestSetupStateWriterTests : public ::testing::Test {
  protected:
    void setMasker(std::string s) { state.masker = std::move(s); }

    void write() { writer.write(state); }

    void assertLabeledEntryWritten(
        TestSetupStateItem item, const std::string &entry) {
        using std::string_literals::operator""s;
        assertWrittenContains(name(item) + ": "s + entry + "\n");
    }

    auto written() -> const auto & { return stub.written(); }

    void assertWrittenContains(const std::string &s) {
        assertTrue(written().contains(s));
    }

  private:
    WriterStub stub;
    TestSetupStateWriter writer{stub};
    TestSetupState state;
};

TEST_F(TestSetupStateWriterTests, writesMasker) {
    setMasker("a");
    write();
    assertLabeledEntryWritten(TestSetupStateItem::masker, "a");
}
}
}
