//
// Created by 观鱼 on 2020/9/30.
//
// const double pi = 3.14159265358979323846264338327950288419716939937510582;
// const double _sqrt_pi_2 = 2.506628274631000241612355239340104162693023681640625;

#ifndef NORM_DIST_H
#define NORM_DIST_H

#include <cstdio>
#include <cmath>

namespace flow::model::norm_dist {
    const double _sqrt_2 = 1.414213562373095048801688724;
    const double _sqrt_pi_2 = 2.506628274631000241612355239340104162693023681640625;

    inline double cdf(const double &x) {
        return std::erfc(-x / _sqrt_2) / 2.0;
    }

    inline double pdf(const double &x) {
        return std::exp(-x * x / 2) / _sqrt_pi_2;
    }

    inline double polynomial(const int &n, const double a[], const double &x) {
        // calculate inverts the standard normal CDF
        double value = 0.0;
        for (int i = n - 1; 0 <= i; i--) {
            value = value * x + a[i];
        }
        return value;
    }

    inline double invert_cdf(const double &p) {
        // calculate inverts the standard normal CDF
        static double a[8] = {
                3.3871328727963666080, 1.3314166789178437745e+2,
                1.9715909503065514427e+3, 1.3731693765509461125e+4,
                4.5921953931549871457e+4, 6.7265770927008700853e+4,
                3.3430575583588128105e+4, 2.5090809287301226727e+3};
        static double b[8] = {
                1.0, 4.2313330701600911252e+1,
                6.8718700749205790830e+2, 5.3941960214247511077e+3,
                2.1213794301586595867e+4, 3.9307895800092710610e+4,
                2.8729085735721942674e+4, 5.2264952788528545610e+3};
        static double c[8] = {
                1.42343711074968357734, 4.63033784615654529590,
                5.76949722146069140550, 3.64784832476320460504,
                1.27045825245236838258, 2.41780725177450611770e-1,
                2.27238449892691845833e-2, 7.74545014278341407640e-4};
        static double const1 = 0.180625;
        static double const2 = 1.6;
        static double d[8] = {
                1.0, 2.05319162663775882187,
                1.67638483018380384940, 6.89767334985100004550e-1,
                1.48103976427480074590e-1, 1.51986665636164571966e-2,
                5.47593808499534494600e-4, 1.05075007164441684324e-9};
        static double e[8] = {
                6.65790464350110377720, 5.46378491116411436990,
                1.78482653991729133580, 2.96560571828504891230e-1,
                2.65321895265761230930e-2, 1.24266094738807843860e-3,
                2.71155556874348757815e-5, 2.01033439929228813265e-7};
        static double f[8] = {
                1.0, 5.99832206555887937690e-1,
                1.36929880922735805310e-1, 1.48753612908506148525e-2,
                7.86869131145613259100e-4, 1.84631831751005468180e-5,
                1.42151175831644588870e-7, 2.04426310338993978564e-15};
        double q;
        double r;
        static double split1 = 0.425;
        static double split2 = 5.0;
        double value;

        if (p <= 0.0) {
            value = -1E30;
            return value;
        }

        if (1.0 <= p) {
            value = 1E30;
            return value;
        }

        q = p - 0.5;

        if (fabs(q) <= split1) {
            r = const1 - q * q;
            value = q * polynomial(8, a, r) / polynomial(8, b, r);
        } else {
            if (q < 0.0) {
                r = p;
            } else {
                r = 1.0 - p;
            }

            if (r <= 0.0) {
                value = -1.0;
                printf("waring!!,inverts CDF formula calculate value is error!");
            }

            r = sqrt(-log(r));

            if (r <= split2) {
                r = r - const2;
                value = polynomial(8, c, r) / polynomial(8, d, r);
            } else {
                r = r - split2;
                value = polynomial(8, e, r) / polynomial(8, f, r);
            }

            if (q < 0.0) {
                value = -value;
            }
        }
        return value;
    }

}

#endif //NORM_DIST_H
