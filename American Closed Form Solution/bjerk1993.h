//
// Created by 观鱼 on 2021/5/26.
//

#ifndef BJERK1993_BJERK1993_H
#define BJERK1993_BJERK1993_H

#include<algorithm>
#include <cmath>
#include "norm_dist.h"
#include "bsm.h"

namespace flow::model::bjerk93 {
    double calculate_value(const bool &is_call, const double &S, const double &X, const double &T,
                           const double &b, const double &r, const double &sigma);
}

#endif //BJERK1993_BJERK1993_H
