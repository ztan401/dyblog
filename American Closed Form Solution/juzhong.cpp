//
// Created by 观鱼 on 2021/5/31.
//

#include "juzhong.h"


namespace flow::model::juzhong {
    using flow::model::norm_dist::cdf;
    using flow::model::norm_dist::pdf;

    // Find the critical value Sx
    double find_critical_value(const double &Sx, const double &X, const double &r, const double &q, const double &sigma,
                               const double &T, const int &phi) {
        // If Sx <= 0 impose a penalty.
        double d1, value, h, beta, alpha, lambda;
        if (Sx <= 0.0)
            return 1.0e100;
        else
            d1 = (log(Sx / X) + (r - q + sigma * sigma / 2) * T) / sigma / sqrt(T);

        if (phi == 1) {
            value = flow::model::bsm::calculate_value(true, Sx, X, T, r - q, r, sigma);
        } else if (phi == -1) {
            value = flow::model::bsm::calculate_value(false, Sx, X, T, r - q, r, sigma);
        } else {
            throw std::out_of_range("This part of the code should never be reached.");
        }

        h = 1.0 - exp(-r * T);
        beta = 2.0 * (r - q) / sigma / sigma;
        alpha = 2.0 * r / sigma / sigma;
        if ((r == 0.0) & (phi == 1))
            lambda = ((1.0 - beta) + sqrt((beta - 1.0) * (beta - 1.0) + 8.0 / sigma / sigma / T)) / 2.0;
        else
            lambda = ((1.0 - beta) + phi * sqrt((beta - 1.0) * (beta - 1.0) + 4.0 * alpha / h)) / 2.0;
        return pow((phi * exp(-q * T) * cdf(phi * d1) + lambda * (phi * (Sx - X) - value) / Sx - phi), 2.0);
    }


    // First derivative
    double find_spot_der1(const double &Sx, const double &X, const double &r, const double &q, const double &sigma,
                          const double &T, const int &phi, const double &dS) {
        return (find_critical_value(Sx + dS, X, r, q, sigma, T, phi) -
                find_critical_value(Sx - dS, X, r, q, sigma, T, phi)) / (2 * dS);
    }

    // Second derivative
    double find_spot_der2(const double &Sx, const double &X, const double &r, const double &q, const double &sigma,
                          const double &T, const int &phi, const double &dS) {
        return (find_critical_value(Sx + dS, X, r, q, sigma, T, phi) -
                2.0 * find_critical_value(Sx, X, r, q, sigma, T, phi) +
                find_critical_value(Sx - dS, X, r, q, sigma, T, phi)) / (dS * dS);
    }

    // Newton's method
    double newton(double S0, const double &X, const double &r, const double &q, const double &sigma, const double &T,
                  const int &phi, const double &dS, const int &max_steps, const double &tol) {
        int k = 0;
        double diff = 1.5 * tol;
        double start = X;
        double S1 = start - find_spot_der1(start, X, r, q, sigma, T, phi, dS) /
                            find_spot_der2(start, X, r, q, sigma, T, phi, dS);
        while (k < max_steps & diff > tol) {
            S0 = S1;
            S1 = S0 - find_spot_der1(S0, X, r, q, sigma, T, phi, dS) / find_spot_der2(S0, X, r, q, sigma, T, phi, dS);
            k += 1;
            diff = abs(S0 - S1);
        }
        return S1;
    }

    double calculate_value(const bool &is_call, const double &S, const double &X, const double &T, const double &b,
                           const double &r, const double &sigma, const int &max_steps, const double &precision,
                           const double &diff) {
        int phi;
        if (is_call) {
            phi = 1;
        } else {
            phi = -1;
        }
        double q = r - b;


        // Find the critical value using Newton's method
        double Sx = newton(S, X, r, q, sigma, T, phi, diff, max_steps, precision);

        // European Black Scholes prices with S = S*
        double eval_value = flow::model::bsm::calculate_value(is_call, Sx, X, T, b, r, sigma);
//        double eval_value = BS.BSPrice(Sx, X, r, q, sigma, T, phi);

        // Required parameters
        double h = (1.0 - exp(-r * T));
        double alpha = 2.0 * r / sigma / sigma;
        double beta = 2.0 * (r - q) / sigma / sigma;
        double hAh = double(phi) * (Sx - X) - eval_value;
        double d1 = (log(Sx / X) + (r - q + sigma * sigma / 2.0) * T) / sigma / sqrt(T);
        double d2 = d1 - sigma * sqrt(T);

        // The case r=0 for calls is treated separately.
        double lambda, bv, c, lambdap, dVdh;
        if ((r == 0.0) & (phi == 1)) {
            lambda = ((1.0 - beta) + sqrt((beta - 1.0) * (beta - 1.0) + 8.0 / sigma / sigma / T)) / 2.0;
            bv = -2.0 / (sigma * sigma * sigma * sigma) / (T * T) /
                 ((beta - 1.0) * (beta - 1.0) + 8.0 / sigma / sigma / T);
            c = -1.0 / sqrt((beta - 1.0) * (beta - 1.0) + 8.0 / sigma / sigma / T) *
                (Sx * pdf(d1) * exp(-q * T) / hAh / sigma / sqrt(T) - 2.0 * q * Sx * cdf(d1) * exp(-q * T) +
                 2.0 / sigma / sigma / T - 4 / (sigma * sigma * sigma * sigma) / (T * T) /
                                           ((beta - 1.0) * (beta - 1.0) + 8.0 / sigma / sigma / T));
        } else {
            lambda = ((1.0 - beta) + phi * sqrt((beta - 1.0) * (beta - 1.0) + 4.0 * alpha / h)) / 2.0;
            lambdap = -phi * alpha / (h * h) / sqrt((beta - 1.0) * (beta - 1.0) + 4.0 * alpha / h);
            dVdh = Sx * pdf(d1) * sigma * exp((r - q) * T) / 2.0 / r / sqrt(T) -
                   phi * q * Sx * cdf(phi * d1) * exp((r - q) * T) / r + phi * X * cdf(phi * d2);
            bv = (1.0 - h) * alpha * lambdap / 2.0 / (2.0 * lambda + beta - 1.0);
            c = -(1.0 - h) * alpha / (2.0 * lambda + beta - 1.0) *
                (1.0 / hAh * dVdh + 1.0 / h + lambdap / (2.0 * lambda + beta - 1.0));
        }
        double xv = bv * (log(S / Sx)) * (log(S / Sx)) + c * log(S / Sx);

        // European option prices with S = Spot
        double value = flow::model::bsm::calculate_value(is_call, S, X, T, b, r, sigma);
//        double value = BS.BSPrice(S, X, r, q, sigma, T, phi);

        // Return the prices.
        if (phi * (Sx - S) > 0.0)
            return value + hAh * pow(S / Sx, lambda) / (1.0 - xv);
        else
            return phi * (S - X);
    }

}