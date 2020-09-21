#ifndef AV_SPEECH_IN_NOISE_AV_SPEECH_IN_NOISE_INCLUDE_AV_SPEECH_IN_NOISE_INTERFACE_HPP_
#define AV_SPEECH_IN_NOISE_AV_SPEECH_IN_NOISE_INCLUDE_AV_SPEECH_IN_NOISE_INTERFACE_HPP_

#define AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(a)               \
    virtual ~a() = default;                                                    \
    a() = default;                                                             \
    a(const a &) = delete;                                                     \
    a(a &&) = delete;                                                          \
    auto operator=(const a &)->a & = delete;                                   \
    auto operator=(a &&)->a & = delete

#endif
