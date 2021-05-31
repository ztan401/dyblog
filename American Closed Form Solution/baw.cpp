
#include "baw.h"
#include "bsm.h"

namespace flow::model::baw {
    using flow::model::norm_dist::cdf;
    using flow::model::norm_dist::pdf;
    const double error_limit = 0.000001;

    // replace nan
    inline double nan_to_num(double& x) {
        if (std::isnan(x) or std::isinf(x)) {
            return 1e-16;
        }
        else {
            return x;
        }
    }

    // Newton Raphson algorithm to solve for the critical commodity price for a Call
    double Kc(const double &X, const double &T, const double &r, const double &b, const double &sigma) {
        // Calculation of seed value, Si
        double N = 2. * b / pow(sigma, 2);
        double m = 2. * r / pow(sigma, 2);
        double q2u = (-(N - 1.) + sqrt(pow((N - 1), 2) + 4. * m)) / 2.;
        double su = X / (1. - 1. / q2u);
        double h2 = -(b * T + 2 * sigma * sqrt(T)) * X / (su - X);
        double Si = X + (su - X) * (1 - exp(h2));

        double k = 2. * r / (pow(sigma, 2) * (1. - exp(-r * T)));
        double d1 = (log(Si / X) + (b + pow(sigma, 2) / 2) * T) / (sigma * sqrt(T));
        double Q2 = (-(N - 1) + sqrt(pow((N - 1), 2) + 4 * k)) / 2.;
        double LHS = Si - X;
        double RHS = flow::model::bsm::calculate_value(true, Si, X, T, b, r, sigma) +
                     (1 - exp((b - r) * T) * cdf(d1)) * Si / Q2;
        double bi =
                exp((b - r) * T) * cdf(d1) * (1 - 1 / Q2) + (1 - exp((b - r) * T) * cdf(d1) / (sigma * sqrt(T))) / Q2;

        k = 2 * r / (pow(sigma, 2) * (1 - exp(-r * T)));
        d1 = (log(Si / X) + (b + pow(sigma, 2) / 2) * T) / (sigma * sqrt(T));
        Q2 = (-(N - 1) + sqrt(pow((N - 1), 2) + 4 * k)) / 2;
        LHS = Si - X;
        RHS = flow::model::bsm::calculate_value(true, Si, X, T, b, r, sigma) +
              (1 - exp((b - r) * T) * cdf(d1)) * Si / Q2;
        bi = exp((b - r) * T) * cdf(d1) * (1 - 1 / Q2) + (1 - exp((b - r) * T) * cdf(d1) / (sigma * sqrt(T))) / Q2;

        while (abs(LHS - RHS) / X > error_limit) {
            Si = (X + RHS - bi * Si) / (1 - bi);
            d1 = (log(Si / X) + (b + pow(sigma, 2) / 2) * T) / (sigma * sqrt(T));
            LHS = Si - X;
            RHS = flow::model::bsm::calculate_value(true, Si, X, T, b, r, sigma) +
                  (1 - exp((b - r) * T) * cdf(d1)) * Si / Q2;
            bi = exp((b - r) * T) * cdf(d1) * (1 - 1 / Q2) + (1 - exp((b - r) * T) * pdf(d1) / (sigma * sqrt(T))) / Q2;
        }
        return Si;
    }

    double calculate_call_value(const double &S, const double &X, const double &T, const double &b, const double &r,
                                const double &sigma) {
        if (b >= r) {
            return flow::model::bsm::calculate_value(true, S, X, T, b, r, sigma);
        }

        double Sk = Kc(X, T, r, b, sigma);
        double N = 2. * b / pow(sigma, 2);
        double k = 2. * r / (pow(sigma, 2) * (1. - exp(-r * T)));
        double d1 = (log(Sk / X) + (b + pow(sigma, 2) / 2.) * T) / (sigma * sqrt(T));
        double Q2 = (-(N - 1.) + sqrt(pow((N - 1.), 2) + 4. * k)) / 2.;
        double a2 = (Sk / Q2) * (1 - exp((b - r) * T) * cdf(d1));

        if (S < Sk) {
            return flow::model::bsm::calculate_value(true, S, X, T, b, r, sigma) + a2 * pow((S / Sk), Q2);
        } else {
            return S - X;
        }
    }

    // Newton Raphson algorithm to solve for the critical commodity price for a Put
    double Kp(const double &X, const double &T, const double &r, const double &b, const double &sigma) {
        // Calculation of seed value, Si
        double N = 2. * b / pow(sigma, 2);
        double m = 2. * r / pow(sigma, 2);
        double q1u = (-(N - 1.) - sqrt(pow(N - 1., 2) + 4. * m)) / 2.;
        double su = X / (1. - 1. / q1u);
        double h1 = (b * T - 2. * sigma * sqrt(T)) * X / (X - su);
        double Si = su + (X - su) * exp(h1);

        double k = 2. * r / (pow(sigma, 2) * (1. - exp(-r * T)));
        double d1 = (log(Si / X) + (b + pow(sigma, 2) / 2.) * T) / (sigma * sqrt(T));
        double Q1 = (-(N - 1.) - sqrt(pow(N - 1., 2) + 4. * k)) / 2.;
        double LHS = X - Si;
        double RHS = flow::model::bsm::calculate_value(false, Si, X, T, b, r, sigma) -
                     (1. - exp((b - r) * T) * cdf(-d1)) * Si / Q1;
        double bi = -exp((b - r) * T) * cdf(-d1) * (1. - 1. / Q1) -
                    (1. + exp((b - r) * T) * pdf(-d1) / (sigma * sqrt(T))) / Q1;

        // Newton Raphson algorithm for finding critical price Si
        while (abs(LHS - RHS) / X > error_limit) {
            Si = (X - RHS + bi * Si) / (1 + bi);
            d1 = (log(Si / X) + (b + pow(sigma, 2) / 2) * T) / (sigma * sqrt(T));
            LHS = X - Si;
            RHS = flow::model::bsm::calculate_value(false, Si, X, T, b, r, sigma) -
                  (1 - exp((b - r) * T) * cdf(-d1)) * Si / Q1;
            bi = -exp((b - r) * T) * cdf(-d1) * (1 - 1 / Q1) -
                 (1 + exp((b - r) * T) * cdf(-d1) / (sigma * sqrt(T))) / Q1;
        }
        return Si;
    }

    double calculate_put_value(const double &S, const double &X, const double &T, const double &b, const double &r,
                               const double &sigma) {
        double Sk = Kp(X, T, r, b, sigma);
        double N = 2. * b / pow(sigma, 2);
        double k = 2. * r / (pow(sigma, 2) * (1. - exp(-r * T)));
        double d1 = (log(Sk / X) + (b + pow(sigma, 2) / 2.) * T) / (sigma * sqrt(T));
        double Q1 = (-(N - 1.) - sqrt(pow(N - 1., 2) + 4. * k)) / 2.;
        double a1 = -(Sk / Q1) * (1 - exp((b - r) * T) * cdf(-d1));

        if (S > Sk) {
            return flow::model::bsm::calculate_value(false, S, X, T, b, r, sigma) + a1 * pow(S / Sk, Q1);
        } else {
            return X - S;
        }
    }

    double calculate_value(const bool &is_call, const double &S, const double &X, const double &T,
                           const double &b, const double &r, const double &sigma) {

        double value;
        if (is_call) {
            value = calculate_call_value(S, X, T, b, r, sigma);
        } else {
            value = calculate_put_value(S, X, T, b, r, sigma);
        }
        return nan_to_num(value);
    }
}



    
    