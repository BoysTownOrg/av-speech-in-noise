#import "objective-c-bridge.h"
#include "Foundation-utility.h"

#include <av-speech-in-noise/ui/TestSettingsInterpreter.hpp>

#include <fstream>
#include <string_view>

// https://stackoverflow.com/a/116220
static auto read_file(std::string_view path) -> std::string {
    constexpr auto read_size = std::size_t{4096};
    auto stream = std::ifstream{path};
    stream.exceptions(std::ios_base::badbit);

    auto out = std::string{};
    auto buf = std::string(read_size, '\0');
    while (stream.read(&buf[0], read_size)) {
        out.append(buf, 0, stream.gcount());
    }
    out.append(buf, 0, stream.gcount());
    return out;
}

@implementation AvSpeechInNoiseUtility
+ (NSString *)meta:(NSString *)stem withExtension:(NSString *)extension {
    return av_speech_in_noise::nsString(
        av_speech_in_noise::TestSettingsInterpreterImpl::meta(read_file(
            av_speech_in_noise::resourceUrl(av_speech_in_noise::string(stem),
                av_speech_in_noise::string(extension))
                .path)));
}

+ (NSString *)resourcePath:(NSString *)stem
             withExtension:(NSString *)extension {
    return av_speech_in_noise::nsString(av_speech_in_noise::resourceUrl(
        av_speech_in_noise::string(stem), av_speech_in_noise::string(extension))
                                            .path);
}
@end