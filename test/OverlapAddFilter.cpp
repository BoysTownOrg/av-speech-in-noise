#include <gsl/gsl>
#include <memory>
#include <complex>
#include <vector>

namespace av_speech_in_noise {
using index_type = typename gsl::index;

template <typename T> using signal_type = gsl::span<T>;

template <typename T> using const_signal_type = signal_type<const T>;

template <typename T>
using signal_iterator_type = typename signal_type<T>::iterator;

template <typename T>
using const_signal_iterator_type = typename const_signal_type<T>::iterator;

template <typename T>
using signal_reverse_iterator_type = typename signal_type<T>::reverse_iterator;

template <typename T> using complex_type = std::complex<T>;

template <typename T> using complex_signal_type = signal_type<complex_type<T>>;

template <typename T>
using const_complex_signal_type = const_signal_type<complex_type<T>>;

template <typename T> using buffer_type = std::vector<T>;

template <typename T>
using buffer_iterator_type = typename buffer_type<T>::iterator;

template <typename T>
using buffer_reverse_iterator_type = typename buffer_type<T>::reverse_iterator;

template <typename T> using complex_buffer_type = buffer_type<complex_type<T>>;

template <typename T> auto size(const signal_type<T> &x) -> index_type {
    return x.size();
}

template <typename T> auto size(const const_signal_type<T> &x) -> index_type {
    return x.size();
}

template <typename T>
auto element(const signal_type<T> &x, index_type i) -> T & {
    return at(x, i);
}

template <typename T>
auto element(const const_signal_type<T> &x, index_type i) -> const T & {
    return at(x, i);
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

template <typename T> void shiftLeft(signal_type<T> x, index_type n) {
    for (index_type i{0}; i < size(x) - n; ++i)
        element(x, i) = element(x, i + n);
    zero<T>(rbegin(x), rbegin(x) + n);
}

template <typename T>
void addFirstToSecond(const_signal_type<T> x, signal_type<T> y) {
    transform(begin(y), end(y), begin(x), begin(y), std::plus<>{});
}

template <typename T>
void multiplyFirstToSecond(const_signal_type<T> x, signal_type<T> y) {
    std::transform(begin(y), end(y), begin(x), begin(y), std::multiplies<>{});
}

template <typename T>
void copyFirstToSecond(const_signal_iterator_type<T> sourceBegin,
    const_signal_iterator_type<T> sourceEnd,
    signal_iterator_type<T> destination) {
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
void transform(const complex_buffer_type<T> &first,
    const complex_buffer_type<T> &second, buffer_type<T> &out,
    std::function<T(const complex_type<T> &, const complex_type<T> &)> f) {
    transform(begin(first), end(first), begin(second), begin(out), f);
}

template <typename T> class Filter {
  public:
    virtual ~Filter() = default;
    virtual void filter(signal_type<T>) = 0;

    class Factory {
      public:
        virtual ~Factory() = default;
        virtual auto make() -> std::shared_ptr<Filter> = 0;
    };
};

template <typename T> class OverlapAdd {
  public:
    explicit OverlapAdd(index_type N);
    void add(const_signal_type<T> x);
    void next(signal_type<T> y);

  private:
    buffer_type<T> buffer;
};

template <typename T> OverlapAdd<T>::OverlapAdd(index_type N) : buffer(N) {}

template <typename T> void OverlapAdd<T>::add(const_signal_type<T> x) {
    addFirstToSecond<T>(x, buffer);
}

template <typename T> void OverlapAdd<T>::next(signal_type<T> y) {
    copyFirstToSecond<T>(buffer, y, size(y));
    shiftLeft<T>(buffer, size(y));
}

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

template <typename T> class OverlapAddFilter : public Filter<T> {
  public:
    OverlapAddFilter(const buffer_type<T> &b,
        typename FourierTransformer<T>::Factory &factory);
    void filter(signal_type<T> x) override;

  private:
    void filter_(signal_type<T> x);

    OverlapAdd<T> overlap;
    complex_buffer_type<T> complexBuffer;
    complex_buffer_type<T> H;
    buffer_type<T> realBuffer;
    std::shared_ptr<FourierTransformer<T>> transformer;
    index_type L;
};

constexpr auto nearestGreaterPowerTwo(index_type n) -> index_type {
    int power{1};
    while ((n >>= 1) != 0U)
        ++power;
    return 1 << power;
}

template <typename T> void resize(buffer_type<T> &x, index_type n) {
    x.resize(n);
}

template <typename T> auto N(const buffer_type<T> &b) -> index_type {
    return nearestGreaterPowerTwo(size<T>(b));
}

template <typename T>
auto dftBufferLength(const buffer_type<T> &b) -> index_type {
    return N(b) / 2 + 1;
}

template <typename T>
void dft(const std::shared_ptr<FourierTransformer<T>> &transformer,
    signal_type<T> x, complex_signal_type<T> X) {
    transformer->dft(x, X);
}

template <typename T>
OverlapAddFilter<T>::OverlapAddFilter(
    const buffer_type<T> &b, typename FourierTransformer<T>::Factory &factory)
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
    multiplyFirstToSecond<complex_type<T>>(H, complexBuffer);
    transformer->idft(complexBuffer, realBuffer);
    overlap.add(realBuffer);
    overlap.next(x);
}
}

#include "assert-utility.hpp"
#include <gtest/gtest.h>

namespace av_speech_in_noise {
namespace {
template <typename T> void copy(const_signal_type<T> x, signal_type<T> y) {
    std::copy(x.begin(), x.end(), y.begin());
}

template <typename T>
void resizeToMatch(std::vector<T> &x, const_signal_type<T> y) {
    x.resize(size(y));
}

template <typename T>
class FourierTransformerStub : public FourierTransformer<T> {
    using complex_buffer_type = std::vector<complex_type<T>>;
    using real_buffer_type = std::vector<T>;
    std::vector<std::vector<T>> dftReals_;
    complex_buffer_type dftComplex_;
    complex_buffer_type idftComplex_;
    real_buffer_type dftReal_;
    real_buffer_type idftReal_;

  public:
    [[nodiscard]] auto dftReal() const { return dftReal_; }

    [[nodiscard]] auto dftReals(size_t n) const { return dftReals_.at(n); }

    [[nodiscard]] auto idftComplex() const { return idftComplex_; }

    void dft(signal_type<T> x, complex_signal_type<T> y) override {
        resizeToMatch<T>(dftReal_, x);
        copy<T>(x, dftReal_);
        copy<complex_type<T>>(dftComplex_, y);
        dftReals_.push_back(dftReal_);
    }

    void setDftComplex(complex_buffer_type x) { dftComplex_ = std::move(x); }

    void setIdftReal(real_buffer_type x) { idftReal_ = std::move(x); }

    void idft(complex_signal_type<T> x, signal_type<T> y) override {
        resizeToMatch<complex_type<T>>(idftComplex_, x);
        copy<complex_type<T>>(x, idftComplex_);
        copy<T>(idftReal_, y);
    }

    class FactoryStub : public FourierTransformer<T>::Factory {
        std::shared_ptr<FourierTransformer<T>> transform;
        index_type N_{};

      public:
        explicit FactoryStub(std::shared_ptr<FourierTransformer<T>> transform)
            : transform{std::move(transform)} {}

        auto make(index_type N)
            -> std::shared_ptr<FourierTransformer<T>> override {
            N_ = N;
            return transform;
        }

        [[nodiscard]] auto N() const { return N_; }
    };
};

class OverlapAddFilterTests : public ::testing::Test {
  protected:
    std::shared_ptr<FourierTransformerStub<double>> fourierTransformer_ =
        std::make_shared<FourierTransformerStub<double>>();
    FourierTransformerStub<double>::FactoryStub factory{fourierTransformer_};
    std::vector<double> b;
    std::vector<double> signal;

    auto construct() -> OverlapAddFilter<double> { return {b, factory}; }

    void assertDftRealEquals(const std::vector<double> &x) {
        assertEqual(x, fourierTransformer_->dftReal());
    }

    void assertIdftComplexEquals(const std::vector<complex_type<double>> &x) {
        assertEqual(x, fourierTransformer_->idftComplex());
    }

    void setTapCount(size_t n) { b.resize(n); }

    void setDftComplex(std::vector<complex_type<double>> x) {
        fourierTransformer_->setDftComplex(std::move(x));
    }

    void setIdftReal(std::vector<double> x) {
        fourierTransformer_->setIdftReal(std::move(x));
    }

    void filter(OverlapAddFilter<double> &overlapAdd) {
        overlapAdd.filter(signal);
    }

    void assertXEquals(const std::vector<double> &x_) {
        assertEqual(x_, signal);
    }

    void resizeX(size_t n) { signal.resize(n); }

    void assertDftRealEquals(const std::vector<double> &x, size_t n) {
        assertEqual(x, fourierTransformer_->dftReals(n));
    }
};

#define OVERLAP_ADD_FILTER_TEST(a) TEST_F(OverlapAddFilterTests, a)

OVERLAP_ADD_FILTER_TEST(
    constructorCreatesTransformWithSizeGreaterThanTapsNearestPowerTwo) {
    b = {1, 2, 3};
    construct();
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(index_type{4}, factory.N());
}

OVERLAP_ADD_FILTER_TEST(
    constructorTransformsTapsZeroPaddedToNearestGreaterPowerTwo) {
    b = {1, 2, 3};
    construct();
    assertDftRealEquals({1, 2, 3, 0});
}

OVERLAP_ADD_FILTER_TEST(filterPassesEachBlockLSamplesToTransformZeroPaddedToN) {
    setTapCount(4 - 1);
    auto overlapAdd = construct();
    signal = {5, 6, 7, 8, 9, 10};
    filter(overlapAdd);
    assertDftRealEquals({5, 6, 0, 0}, 1);
    assertDftRealEquals({7, 8, 0, 0}, 2);
    assertDftRealEquals({9, 10, 0, 0}, 3);
}

OVERLAP_ADD_FILTER_TEST(
    filterPassesEachBlockLSamplesToTransformZeroPaddedToN2) {
    setTapCount(4 - 1);
    auto overlapAdd = construct();
    signal = {5, 6, 7};
    filter(overlapAdd);
    assertDftRealEquals({5, 6, 0, 0}, 1);
    assertDftRealEquals({7, 0, 0, 0}, 2);
}

OVERLAP_ADD_FILTER_TEST(filterPassesTransformProductToInverseTransform) {
    setTapCount(8 - 1);
    setDftComplex({5, 6, 7, 8, 9});
    auto overlapAdd = construct();
    setDftComplex({11, 12, 13, 14, 15});
    resizeX(2);
    filter(overlapAdd);
    assertIdftComplexEquals({5 * 11, 6 * 12, 7 * 13, 8 * 14, 9 * 15});
}

OVERLAP_ADD_FILTER_TEST(filterOverlapAddsInverseTransform) {
    setTapCount(8 - 1);
    setDftComplex({0, 0, 0, 0, 0});
    auto overlapAdd = construct();
    resizeX(2);
    setIdftReal({5, 6, 7, 8, 9, 10, 11, 12});
    filter(overlapAdd);
    assertXEquals({5, 6});
}

OVERLAP_ADD_FILTER_TEST(filterOverlapAddsInverseTransform2) {
    setTapCount(4 - 1);
    setDftComplex({0, 0, 0});
    auto overlapAdd = construct();
    resizeX(4);
    setIdftReal({5, 6, 7, 8});
    filter(overlapAdd);
    assertXEquals({5, 6, 5 + 7, 6 + 8});
}

OVERLAP_ADD_FILTER_TEST(filterOverlapAddsInverseTransform3) {
    setTapCount(4 - 1);
    setDftComplex({0, 0, 0});
    auto overlapAdd = construct();
    resizeX(2);
    setIdftReal({5, 6, 7, 8});
    filter(overlapAdd);
    assertXEquals({5, 6});
    setIdftReal({9, 10, 11, 12});
    filter(overlapAdd);
    assertXEquals({9 + 7, 10 + 8});
}

OVERLAP_ADD_FILTER_TEST(filterOverlapAddsInverseTransform4) {
    setTapCount(4 - 1);
    setDftComplex({0, 0, 0});
    auto overlapAdd = construct();
    resizeX(3);
    setIdftReal({5, 6, 7, 8});
    filter(overlapAdd);
    assertXEquals({5, 6, 5 + 7});
}

OVERLAP_ADD_FILTER_TEST(filterOverlapAddsInverseTransform5) {
    setTapCount(4 - 1);
    setDftComplex({0, 0, 0});
    auto overlapAdd = construct();
    resizeX(3);
    setIdftReal({5, 6, 7, 8});
    filter(overlapAdd);
    assertXEquals({5, 6, 5 + 7});
    setIdftReal({9, 10, 11, 12});
    filter(overlapAdd);
    assertXEquals({6 + 8 + 9, 7 + 10, 8 + 11 + 9});
}
}
}
