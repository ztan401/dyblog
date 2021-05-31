//
// Created by 观鱼 on 2021/5/26.
//

#include "bjerk2002.h"

namespace flow::model::bjerk02 {
    using flow::model::norm_dist::cdf;
    using flow::model::bi_norm_dist::bi_cdf;

    // replace nan
    inline double nan_to_num(double& x) {
        if (std::isnan(x) or std::isinf(x)) {
            return 1e-16;
        }
        else {
            return x;
        }
    }

    // Bjerksund and Stensland (2002) "phi" function
    double calculate_phi(double S, double T, double gamma, double h, double i, double r, double b, double sigma) {
        // Equation 8
        double lambda = -r + gamma * b + 0.5 * gamma * (gamma - 1.0) * sigma * sigma;
        // Equation 9
        double kappa = 2.0 * b / sigma / sigma + (2.0 * gamma - 1.0);

        // Equation 7 for the phi function
        double d1 = -(log(S / h) + (b + (gamma - 0.5) * sigma * sigma) * T) / sigma / sqrt(T);
        double d2 = -(log(i * i / S / h) + (b + (gamma - 0.5) * sigma * sigma) * T) / sigma / sqrt(T);

        // Return phi
        return exp(lambda * T) * pow(S, gamma) * (cdf(d1) - pow(i / S, kappa) * cdf(d2));
    }

    // Bjerksund and Stensland  (2002)"psi" function
    double
    calculate_ksi(double S, double T2, double gamma, double h, double I2, double I1, double t1, double r, double b,
                  double sigma) {
        double e1 = -(log(S / I1) + (b + (gamma - 0.5) * sigma * sigma) * t1) / sigma / sqrt(t1);
        double e2 = -(log(I2 * I2 / S / I1) + (b + (gamma - 0.5) * sigma * sigma) * t1) / sigma / sqrt(t1);
        double e3 = -(log(S / I1) - (b + (gamma - 0.5) * sigma * sigma) * t1) / sigma / sqrt(t1);
        double e4 = -(log(I2 * I2 / S / I1) - (b + (gamma - 0.5) * sigma * sigma) * t1) / sigma / sqrt(t1);

        double f1 = -(log(S / h) + (b + (gamma - 0.5) * sigma * sigma) * T2) / sigma / sqrt(T2);
        double f2 = -(log(I2 * I2 / S / h) + (b + (gamma - 0.5) * sigma * sigma) * T2) / sigma / sqrt(T2);
        double f3 = -(log(I1 * I1 / S / h) + (b + (gamma - 0.5) * sigma * sigma) * T2) / sigma / sqrt(T2);
        double f4 = -(log(S * I1 * I1 / h / I2 / I2) + (b + (gamma - 0.5) * sigma * sigma) * T2) / sigma / sqrt(T2);

        // Correlation
        double rho = sqrt(t1 / T2);

        // Equation 8
        double lambda = -r + gamma * b + 0.5 * gamma * (gamma - 1.0) * sigma * sigma;
        // Equation 9
        double kappa = 2.0 * b / sigma / sigma + (2.0 * gamma - 1.0);

        // Return psi on page 5
        double psi = exp(lambda * T2) * pow(S, gamma) * (
                bi_cdf(e1, f1, rho) -
                bi_cdf(e2, f2, rho) * pow(I2 / S, kappa) -
                bi_cdf(e3, f3, -rho) * pow(I1 / S, kappa) +
                bi_cdf(e4, f4, -rho) * pow(I1 / I2, kappa));
        return psi;
    }


    // Bjerksund and Stensland (2002) two-step boundary call approximation
    double calculate_call_value(double S, double X, double T, double b, double r, double sigma) {
        // If b >= r then it's not optimal to exercise before expiry
        if (b >= r) {
            return flow::model::bsm::calculate_value(true, S, X, T, b, r, sigma);
        } else {
            // Equation (6)
            double beta = (0.5 - b / sigma / sigma) + sqrt(pow(b / sigma / sigma - 0.5, 2.0) + 2.0 * r / sigma / sigma);

            // Equation (10) through (13)
            double Binf = X * beta / (beta - 1.0);
            double B0 = std::max(X, X * r / (r - b));
            double hT = -(b * T + 2.0 * sigma * sqrt(T)) * X * X / B0 / (Binf - B0);
            double I = B0 + (Binf - B0) * (1.0 - exp(hT));

            // Determine boundary condition
            if (S >= I)
                return S - X;
            else {
                // Equation (5) alpha(I)
                double alphaX = (I - X) * pow(I, -beta);

                // Equation (16)
                double t = 0.5 * (sqrt(5.0) - 1.0) * T;

                // Equation (18)
                double hTt = -(b * (T - t) + 2.0 * sigma * sqrt(T - t)) * X * X / B0 / (Binf - B0);

                // Redefine x and alpha(x) in terms of t
                double x = B0 + (Binf - B0) * (1.0 - exp(hTt));
                double alphax = (x - X) * pow(x, -beta);

                // Call price Equation (4)
                return alphaX * pow(S, beta)
                       - alphaX * calculate_phi(S, t, beta, I, I, r, b, sigma)
                       + calculate_phi(S, t, 1.0, I, I, r, b, sigma)
                       - calculate_phi(S, t, 1.0, x, I, r, b, sigma)
                       - X * calculate_phi(S, t, 0.0, I, I, r, b, sigma)
                       + X * calculate_phi(S, t, 0.0, x, I, r, b, sigma)
                       + alphax * calculate_phi(S, t, beta, x, I, r, b, sigma)
                       - alphax * calculate_ksi(S, T, beta, x, I, x, t, r, b, sigma)
                       + calculate_ksi(S, T, 1.0, x, I, x, t, r, b, sigma)
                       - calculate_ksi(S, T, 1.0, X, I, x, t, r, b, sigma)
                       - X * calculate_ksi(S, T, 0.0, x, I, x, t, r, b, sigma)
                       + X * calculate_ksi(S, T, 0.0, X, I, x, t, r, b, sigma);
            }
        }
    }

    // Bjerksund and Stensland (2002) two-step boundary call and put approximation
    double calculate_value(const bool &is_call, const double &S, const double &X, const double &T,
                           const double &b, const double &r, const double &sigma) {
        double value;
        if (is_call) {
            value = calculate_call_value(S, X, T, b, r, sigma);
        } else {
            value = calculate_call_value(X, S, T, -b, r - b, sigma);
        }
        return nan_to_num(value);
    }
}

