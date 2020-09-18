#include "OverlapAddFilter.hpp"

namespace av_speech_in_noise {
template <typename T> auto size(const signal_type<T> &x) -> index_type {
    return x.size();
}

template <typename T> auto size(const const_signal_type<T> &x) -> index_type {
    return x.size();
}

template <typename T> void zero(signal_type<T> x) {
    std::fill(begin(x), end(x), T{0});
}

template <typename T>
void zero(
    signal_reverse_iterator_type<T> b, signal_reverse_iterator_type<T> e) {
    fill(b, e, T{0});
}

template <typename T>
void zero(buffer_iterator_type<T> b, buffer_iterator_type<T> e) {
    fill(b, e, T{0});
}

template <typename T>
void zero(
    buffer_reverse_iterator_type<T> b, buffer_reverse_iterator_type<T> e) {
    fill(b, e, T{0});
}

template <typename T>
void copyFirstToSecond(const_signal_iterator_type<T> sourceBegin,
    const_signal_iterator_type<T> sourceEnd,
    typename signal_type<T>::iterator destination) {
    std::copy(sourceBegin, sourceEnd, destination);
}

template <typename T>
void copyFirstToSecond(
    const_signal_type<T> source, signal_type<T> destination) {
    copyFirstToSecond<T>(begin(source), end(source), begin(destination));
}

template <typename T>
void copyFirstToSecond(
    const_signal_type<T> source, signal_type<T> destination, index_type n) {
    copyFirstToSecond<T>(begin(source), begin(source) + n, begin(destination));
}

template <typename T>
void transform(const std::vector<complex_type<T>> &first,
    const std::vector<complex_type<T>> &second, std::vector<T> &out,
    std::function<T(const complex_type<T> &, const complex_type<T> &)> f) {
    transform(begin(first), end(first), begin(second), begin(out), f);
}

template <typename T> OverlapAdd<T>::OverlapAdd(index_type N) : buffer(N) {}

template <typename T> void OverlapAdd<T>::add(const_signal_type<T> x) {
    transform(
        begin(buffer), end(buffer), begin(x), begin(buffer), std::plus<>{});
}

template <typename T> void shiftLeft(signal_type<T> x, index_type n) {
    for (index_type i{0}; i < size(x) - n; ++i)
        at(x, i) = at(x, i + n);
    zero<T>(rbegin(x), rbegin(x) + n);
}

template <typename T> void OverlapAdd<T>::next(signal_type<T> y) {
    copyFirstToSecond<T>(buffer, y, size(y));
    shiftLeft<T>(buffer, size(y));
}

constexpr auto nearestGreaterPowerTwo(index_type n) -> index_type {
    int power{1};
    while ((n >>= 1) != 0U)
        ++power;
    return 1 << power;
}

template <typename T> void resize(std::vector<T> &x, index_type n) {
    x.resize(n);
}

template <typename T> auto N(const std::vector<T> &b) -> index_type {
    return nearestGreaterPowerTwo(size<T>(b));
}

template <typename T>
auto dftBufferLength(const std::vector<T> &b) -> index_type {
    return N(b) / 2 + 1;
}

template <typename T>
void dft(const std::shared_ptr<FourierTransformer<T>> &transformer,
    signal_type<T> x, complex_signal_type<T> X) {
    transformer->dft(x, X);
}

template <typename T>
OverlapAddFilter<T>::OverlapAddFilter(
    const std::vector<T> &b, typename FourierTransformer<T>::Factory &factory)
    : overlap{N(b)}, complexBuffer(dftBufferLength(b)), H(dftBufferLength(b)),
      realBuffer(N(b)), transformer{factory.make(N(b))}, L{N(b) - size<T>(b) +
                                                             1} {
    copyFirstToSecond<T>(b, realBuffer);
    dft<T>(transformer, realBuffer, H);
}

template <typename T> void OverlapAddFilter<T>::filter(signal_type<T> x) {
    for (index_type j{0}; j < size(x) / L; ++j)
        filter_(x.subspan(j * L, L));
    if (const auto left{size(x) % L})
        filter_(x.last(left));
}

template <typename T> void OverlapAddFilter<T>::filter_(signal_type<T> x) {
    zero<T>(begin(realBuffer) + size(x), end(realBuffer));
    copyFirstToSecond<T>(x, realBuffer);
    dft<T>(transformer, realBuffer, complexBuffer);
    transform(begin(complexBuffer), end(complexBuffer), begin(H),
        begin(complexBuffer), std::multiplies<>{});
    transformer->idft(complexBuffer, realBuffer);
    overlap.add(realBuffer);
    overlap.next(x);
}
template class OverlapAddFilter<double>;
template class OverlapAddFilter<float>;
}