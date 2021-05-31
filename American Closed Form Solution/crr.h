//
// Created by 观鱼 on 2021/5/27.
// CRR Lattice with Dynamic Memory design and  a truncated zero region
//Dynamic Binomial with Truncated zeroes
//Faster with power term
//Got rid of z

//Working.
//Tested for American Call Option and giving the same results for Broadie Detemple (1996) 9.066 p. 1224 Table 2
//The binomial price is 9.06594 where
//int n = 15000;       // Number of steps
//double S = 100.0;      // Spot Price
//double K = 100.0;      // Strike Price
//double T = 3;       // Years to maturity
//double r = 0.03;      // Risk Free Rate
//double q = 0.07;      //double q = 0.03;
//double v = 0.20;
//char PutCall = 'C';
//char OpStyle = 'A';


#ifndef CRR_CRR_H
#define CRR_CRR_H

#include <cmath>
#include <iostream>
#include <algorithm>
#include <vector>
#include <string>

namespace flow::model::crr {
    double calculate_value(const bool &is_call, const double &S, const double &X, const double &T, const double &b,
                           const double &r, const double &sigma, const int &steps = 128);
}

#endif //CRR_CRR_H
