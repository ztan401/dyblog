//
// Created by 观鱼 on 2021/5/26.
//

#include "bi_norm_dist.h"


namespace flow::model::bi_norm_dist {
    using flow::model::norm_dist::cdf;

    // Sign function
    double sign(const double &a) {
        if (a < 0.)
            return -1.0;
        else if (a > 0.)
            return 1.0;
        else
            return 0.0;
    }

    // Support function
    double f(double x, double y, double aprime, double bprime, double rho) {
        double r = aprime * (2.0 * x - aprime) + bprime * (2.0 * y - bprime) + 2.0 * rho * (x - aprime) * (y - bprime);
        return exp(r);
    }

    double bi_cdf(double a, double b, double rho) {
        double pi = 3.14159265359;
        if ((a <= 0.0) && (b <= 0.0) && (rho <= 0.0)) {
            double aprime = a / sqrt(2.0 * (1.0 - rho * rho));
            double bprime = b / sqrt(2.0 * (1.0 - rho * rho));
            double A[] = {0.3253030, 0.4211071, 0.1334425, 0.006374323};
            double B[] = {0.1337764, 0.6243247, 1.3425378, 2.2626645};
            double sum = 0.0;
            for (int i = 0; i < 4; i++)
                for (int j = 0; j < 4; j++)
                    sum = sum + A[i] * A[j] * f(B[i], B[j], aprime, bprime, rho);
            sum = sum * (sqrt(1.0 - rho * rho) / pi);
            return sum;
        } else if (a * b * rho <= 0.0) {
            if ((a <= 0.0) && (b >= 0.0) && (rho >= 0.0))
                return cdf(a) - bi_cdf(a, -1.0 * b, -1.0 * rho);
            else if ((a >= 0.0) && (b <= 0.0) && (rho >= 0.0))
                return cdf(b) - bi_cdf(-1.0 * a, b, -1.0 * rho);
            else if ((a >= 0.0) && (b >= 0.0) && (rho <= 0.0))
                return cdf(a) + cdf(b) - 1.0 + bi_cdf(-1.0 * a, -1.0 * b, rho);
        } else if (a * b * rho >= 0.0) {
            double denum = sqrt(a * a - 2.0 * rho * a * b + b * b);
            double rho1 = ((rho * a - b) * sign(a)) / denum;
            double rho2 = ((rho * b - a) * sign(b)) / denum;
            double delta = (1.0 - sign(a) * sign(b)) / 4.0;
            return bi_cdf(a, 0.0, rho1) + bi_cdf(b, 0.0, rho2) - delta;
        }else{
            throw std::out_of_range("This part of the code should never be reached.");
        }
        throw std::out_of_range("This part of the code should never be reached.");
    }
}