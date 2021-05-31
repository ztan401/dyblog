//
// Created by 观鱼 on 2021/5/25.
//

#ifndef AMERICAN_CLOSED_FORM_SOLUTION_BAW_H
#define AMERICAN_CLOSED_FORM_SOLUTION_BAW_H

#include <cmath>
#include "norm_dist.h"
#include "bsm.h"

namespace flow::model::baw {
    double calculate_value(const bool &is_call, const double &S, const double &X, const double &T,
                           const double &b, const double &r, const double &sigma);
}

#endif //AMERICAN_CLOSED_FORM_SOLUTION_BAW_H
