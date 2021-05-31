//
// Created by 观鱼 on 2020/9/24.
//

#include "bsm.h"

namespace flow::model::bsm {
    using flow::model::norm_dist::cdf;
    using flow::model::norm_dist::pdf;
    using flow::model::norm_dist::invert_cdf;
    using std::abs;
    using std::exp;
    using std::sqrt;

    // replace nan
    inline double nan_to_num(double &x) {
        if (std::isnan(x) or std::isinf(x)) {
            return 1e-16;
        } else {
            return x;
        }
    }

    inline double clip_ttm(const double &x, const double &ttm_clip = 1e-6) {
        return (x < ttm_clip) ? ttm_clip : x;
    }

    inline double clip_iv(const double &x, const double &iv_lower_clip = 1e-4, const double &iv_upper_clip = 10.) {
        if ((x < iv_lower_clip) || (x > iv_upper_clip)) {
            return 1e-16;
        } else {
            return x;
        }
    }

    // use for calculate_iv_newton, clip calculate_vega
    inline double clip_vega(const double &x, const double &vega_clip = 1e-8) {
        return (x < vega_clip) ? vega_clip : x;
    }

    inline double
    calc_d1(const double &S, const double &X, const double &T, const double &b, const double &sigma) {
        return (log(S / X) + (b + sigma * sigma / 2) * T) / (sqrt(T) * sigma);
    }

    inline double
    calc_d2(const double &d1, const double &T, const double &sigma) {
        return d1 - sigma * sqrt(T);
    }

    double
    calculate_call_value(const double &S, const double &X, const double &T, const double &b, const double &r,
                         const double &sigma) {
        double d1 = calc_d1(S, X, T, b, sigma);
        double value = S * exp((b - r) * T) * cdf(d1) - X * exp(-r * T) * cdf(calc_d2(d1, T, sigma));
        return nan_to_num(value);
    }

    double
    calculate_put_value(const double &S, const double &X, const double &T, const double &b, const double &r,
                        const double &sigma) {
        double d1 = calc_d1(S, X, T, b, sigma);
        double d2 = calc_d2(d1, T, sigma);
        double value = X * exp(-r * T) * cdf(-d2) - S * exp((b - r) * T) * cdf(-d1);
        return nan_to_num(value);
    }

    double calculate_value(const bool &is_call, const double &S, const double &X, const double &T,
                           const double &b, const double &r, const double &sigma) {
        if (is_call) {
            return calculate_call_value(S, X, T, b, r, sigma);
        } else {
            return calculate_put_value(S, X, T, b, r, sigma);
        }
    }
}
