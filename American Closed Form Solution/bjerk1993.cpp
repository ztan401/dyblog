//
// Created by 观鱼 on 2021/5/26.
//

#include "bjerk1993.h"

namespace flow::model::bjerk93 {
    using flow::model::norm_dist::cdf;

    // replace nan
    inline double nan_to_num(double& x) {
        if (std::isnan(x) or std::isinf(x)) {
            return 1e-16;
        }
        else {
            return x;
        }
    }

    /// 注意该函数的r和b的顺序
    // Bjerksund and Stensland (1993) "phi" function
    double calculate_phi(double S, double T, double gamma, double h, double i, double r, double b, double sigma) {
        // Equation 8
        double lambda = -r + gamma * b + 0.5 * gamma * (gamma - 1.0) * sigma * sigma;
        // Equation 9
        double kappa = 2.0 * b / sigma / sigma + (2.0 * gamma - 1.0);

        // Equation 7 for the phi function
        double d1 = -(log(S / h) + (b + (gamma - 0.5) * sigma * sigma) * T) / sigma / sqrt(T);
        double d2 = -(log(i * i / S / h) + (b + (gamma - 0.5) * sigma * sigma) * T) / sigma / sqrt(T);

        return exp(lambda * T) * pow(S, gamma) * (cdf(d1) - pow(i / S, kappa) * cdf(d2));
    }

    // Bjerksund and Stensland (1993) call price for flat boundary call approximation
    double calculate_call_value(const double &S, const double &X, const double &T, const double &b, const double &r,
                                const double &sigma) {
        // If b >= r then it's not optimal to exercise before expiry
        if (b >= r)
            return flow::model::bsm::calculate_value(true, S, X, T, b, r, sigma);
        else {
            // Equation (6)
            double beta =
                    (0.5 - b / sigma / sigma) + sqrt(pow((b / sigma / sigma - 0.5), 2.0) + 2.0 * r / sigma / sigma);

            // Equation (10) through (13)
            double Binf = X * beta / (beta - 1.0);
            double B0 = std::max(X, X * r / (r - b));
            double ht = -(b * T + 2.0 * sigma * sqrt(T)) * X * X / B0 / (Binf - B0);
            double i = B0 + (Binf - B0) * (1 - exp(ht));

            // Determine boundary condition
            if (S >= i) {
                return S - X;
            } else {
                // Equation (5)
                double alpha = (i - X) * pow(i, -beta);

                // Call price Equation (4)
                return alpha * pow(S, beta)
                       - alpha * calculate_phi(S, T, beta, i, i, r, b, sigma)
                       + calculate_phi(S, T, 1.0, i, i, r, b, sigma)
                       - calculate_phi(S, T, 1.0, X, i, r, b, sigma)
                       - X * calculate_phi(S, T, 0.0, i, i, r, b, sigma)
                       + X * calculate_phi(S, T, 0.0, X, i, r, b, sigma);
            }
        }
    }

    // Bjerksund and Stensland (1993) call or put price
    double calculate_value(const bool &is_call, const double &S, const double &X, const double &T, const double &b,
                           const double &r, const double &sigma) {
        double value;
        if (is_call) {
            value = calculate_call_value(S, X, T, b, r, sigma);
        } else {
            value = calculate_call_value(X, S, T, -b, r - b, sigma);
        }
        return nan_to_num(value);
    }
}
