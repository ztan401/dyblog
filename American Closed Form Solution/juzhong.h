//
// Created by 观鱼 on 2021/5/31.
//

#ifndef ZJ_JUZHONG_H
#define ZJ_JUZHONG_H

#include <stdexcept>
#include "bsm.h"

namespace flow::model::juzhong {
    double calculate_value(const bool &is_call, const double &S, const double &X, const double &T, const double &b,
                           const double &r, const double &sigma, const int &max_steps = 8192,
                           const double &precision = 1e-8, const double &diff = 0.01);
}

#endif //ZJ_JUZHONG_H
