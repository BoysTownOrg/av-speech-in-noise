#ifndef AV_SPEECH_IN_NOISE_STIMULUS_PLAYERS_INCLUDE_STIMULUS_PLAYERS_OVERLAPADDFILTER_HPP_
#define AV_SPEECH_IN_NOISE_STIMULUS_PLAYERS_INCLUDE_STIMULUS_PLAYERS_OVERLAPADDFILTER_HPP_

#include "MonoSignalProcessor.hpp"
#include <gsl/gsl>
#include <memory>
#include <complex>
#include <vector>

namespace av_speech_in_noise {
using index_type = typename gsl::index;

template <typename T> using signal_type = gsl::span<T>;

template <typename T> using const_signal_type = signal_type<const T>;

template <typename T>
using const_signal_iterator_type = typename const_signal_type<T>::iterator;

template <typename T>
using signal_reverse_iterator_type = typename signal_type<T>::reverse_iterator;

template <typename T> using complex_type = std::complex<T>;

template <typename T> using complex_signal_type = signal_type<complex_type<T>>;

template <typename T>
using buffer_iterator_type = typename std::vector<T>::iterator;

template <typename T>
using buffer_reverse_iterator_type = typename std::vector<T>::reverse_iterator;

template <typename T> class OverlapAdd {
  public:
    explicit OverlapAdd(index_type N);
    void add(const_signal_type<T> x);
    void next(signal_type<T> y);

  private:
    std::vector<T> buffer;
};

template <typename T> class FourierTransformer {
  public:
    virtual ~FourierTransformer() = default;
    virtual void dft(signal_type<T>, complex_signal_type<T>) = 0;
    virtual void idft(complex_signal_type<T>, signal_type<T>) = 0;

    class Factory {
      public:
        virtual ~Factory() = default;
        virtual auto make(index_type N)
            -> std::shared_ptr<FourierTransformer> = 0;
    };
};

template <typename T> class OverlapAddFilter : public MonoSignalProcessor<T> {
  public:
    OverlapAddFilter(const std::vector<T> &b,
        typename FourierTransformer<T>::Factory &factory);
    void process(signal_type<T> x) override;

  private:
    void filter_(signal_type<T> x);

    OverlapAdd<T> overlap;
    std::vector<complex_type<T>> complexBuffer;
    std::vector<complex_type<T>> H;
    std::vector<T> realBuffer;
    std::shared_ptr<FourierTransformer<T>> transformer;
    index_type L;
};

extern template class OverlapAddFilter<float>;
extern template class OverlapAddFilter<double>;
}

#endif
