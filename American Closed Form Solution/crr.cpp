//
// Created by 观鱼 on 2021/5/27.
//

#include "crr.h"


namespace flow::model::crr {
    using std::vector;
    using std::max;

    // replace nan
    inline double nan_to_num(double& x) {
        if (std::isnan(x) or std::isinf(x)) {
            return 1e-16;
        }
        else {
            return x;
        }
    }

    double calculate_call_value(const double &S, const double &X, const double &T, const double &q, const double &r,
                                const double &sigma, const int &steps) {

        // Quantities for the tree
        double dt = T / steps;
        double u = exp(sigma * sqrt(dt));
        double d = 1.0 / u;
        double p = (exp((r - q) * dt) - d) / (u - d);
        int Num = int(log(X / (S * pow(d, steps))) / log(u / d));

        vector<double> option;
        option.resize(steps + 1);  //resize the column
        for (int i = 0; i <= steps; i++) {
            option[i] = max((S * pow(u, (2 * i - steps)) - X), 0.0);
        }

        // Backward recursion through the tree
        for (int j = steps - 1; j >= 0; j--) {
            //Qianru Shang adapted this section for preliminary tests
            int Start = Num - (steps - 1 - j);
            if (j < (steps - Num)) {
                Start = 0;
            }
            for (int i = Start; i <= j; i++) {
                option[i] = max((S * pow(u, (2 * i - j)) - X),
                                exp(-r * dt) * (p * (option[i + 1]) + (1.0 - p) * (option[i])));
            }
        }

        return option[0];
    }


    double calculate_value(const bool &is_call, const double &S, const double &X, const double &T, const double &b,
                           const double &r, const double &sigma, const int &steps) {
        double q = r - b;
        double value;
        if (is_call) {
            value = calculate_call_value(S, X, T, q, r, sigma, steps);
        } else {
            value =calculate_call_value(X, S, T, r, q, sigma, steps);
        }
        return nan_to_num(value);
    }
}

