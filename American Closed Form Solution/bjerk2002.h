//
// Created by 观鱼 on 2021/5/26.
//

#ifndef BJERK2002_BJERK2002_H
#define BJERK2002_BJERK2002_H

#include <cmath>
#include <algorithm>
#include "norm_dist.h"
#include "bi_norm_dist.h"
#include "bsm.h"

namespace flow::model::bjerk02 {
    double calculate_value(const bool &is_call, const double &S, const double &X, const double &T,
                           const double &b, const double &r, const double &sigma);
}

#endif //BJERK2002_BJERK2002_H
