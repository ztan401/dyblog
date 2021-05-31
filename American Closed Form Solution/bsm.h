//
// Created by 观鱼 on 2020/9/24.
//

#ifndef BSM_H
#define BSM_H

#include "norm_dist.h"


namespace flow::model::bsm {
    double calculate_value(const bool &is_call, const double &S, const double &X, const double &T,
                           const double &b, const double &r, const double &sigma);
}
#endif //BSM_H
