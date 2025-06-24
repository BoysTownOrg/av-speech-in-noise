#include "LogisticPsychometricFunction.hpp"
#include <cmath>

auto LogisticPsychometricFunction::operator()(Phi phi, double x) -> double {
    return phi.gamma +
        ((1 - phi.gamma - phi.lambda) /
            (1 + std::exp(-(x - phi.alpha) * phi.beta)));
}
