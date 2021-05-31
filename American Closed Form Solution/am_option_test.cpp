
#include <iostream>
#include <fstream>
#include <vector>
#include "tabulate.hpp"

#include "bsm.h"
#include "crr.h"
#include "baw.h"
#include "juzhong.h"
#include "bjerk1993.h"
#include "bjerk2002.h"

void test_crr_steps() {
    double spot = 100.;
    double strike = 105.;
    double risk_free = 0.03;
    double cost_of_carry = -0.02;
    double sigma = 0.3;

//    std::vector<double> T = {20, 40, 60, 90, 120, 160, 190, 244};
    std::vector<double> T = {20, 40};
    tabulate::Table table;
    table.add_row({"ExpiryTime", "C/P", "steps=64", "256", "1024", "4096", "16384", "65536", "262144"});

    std::vector<double>::iterator expiry_time;
    for (expiry_time = T.begin(); expiry_time != T.end(); expiry_time++) {
        std::cout << "Expiry Time: " << *expiry_time << "Day" << std::endl;
        double expiry_time_ = *expiry_time / 244.;
        table.add_row({
                              std::to_string(*expiry_time),
                              "Call",
                              std::to_string(
                                      flow::model::crr::calculate_value(true, spot, strike, expiry_time_, cost_of_carry,
                                                                        risk_free, sigma, 64)),
                              std::to_string(
                                      flow::model::crr::calculate_value(true, spot, strike, expiry_time_, cost_of_carry,
                                                                        risk_free, sigma, 256)),
                              std::to_string(
                                      flow::model::crr::calculate_value(true, spot, strike, expiry_time_, cost_of_carry,
                                                                        risk_free, sigma, 1024)),
                              std::to_string(
                                      flow::model::crr::calculate_value(true, spot, strike, expiry_time_, cost_of_carry,
                                                                        risk_free, sigma, 4096)),
                              std::to_string(
                                      flow::model::crr::calculate_value(true, spot, strike, expiry_time_, cost_of_carry,
                                                                        risk_free, sigma, 16384)),
                              std::to_string(
                                      flow::model::crr::calculate_value(true, spot, strike, expiry_time_, cost_of_carry,
                                                                        risk_free, sigma, 65536)),
                              std::to_string(
                                      flow::model::crr::calculate_value(true, spot, strike, expiry_time_, cost_of_carry,
                                                                        risk_free, sigma, 262144)),
                      });
        std::cout << table << std::endl;

        table.add_row({
                              std::to_string(*expiry_time),
                              "Put",
                              std::to_string(flow::model::crr::calculate_value(false, spot, strike, expiry_time_,
                                                                               cost_of_carry, risk_free, sigma, 64)),
                              std::to_string(flow::model::crr::calculate_value(false, spot, strike, expiry_time_,
                                                                               cost_of_carry, risk_free, sigma, 256)),
                              std::to_string(flow::model::crr::calculate_value(false, spot, strike, expiry_time_,
                                                                               cost_of_carry, risk_free, sigma, 1024)),
                              std::to_string(flow::model::crr::calculate_value(false, spot, strike, expiry_time_,
                                                                               cost_of_carry, risk_free, sigma, 4096)),
                              std::to_string(flow::model::crr::calculate_value(false, spot, strike, expiry_time_,
                                                                               cost_of_carry, risk_free, sigma, 16384)),
                              std::to_string(flow::model::crr::calculate_value(false, spot, strike, expiry_time_,
                                                                               cost_of_carry, risk_free, sigma, 65536)),
                              std::to_string(flow::model::crr::calculate_value(false, spot, strike, expiry_time_,
                                                                               cost_of_carry, risk_free, sigma,
                                                                               262144)),
                      });
        std::cout << table << std::endl;
    }

    std::cout << table << std::endl;
}


void show() {
    double spot = 100.;
    double strike = 105.;
    double risk_free = 0.03;
    double cost_of_carry = -0.02;
    double sigma = 0.3;
    std::vector<double> T = {20, 40, 60, 90, 120, 160, 190, 244};

    tabulate::Table table;
    table.add_row({"ExpiryTime", "C/P", "BSM", "CRR", "BAW", "JuZhong", "Bjerk93", "Bjerk02"});

    std::vector<double>::iterator expiry_time;
    for (expiry_time = T.begin(); expiry_time != T.end(); expiry_time++) {
        double expiry_time_ = *expiry_time / 244.;
        table.add_row({
                              std::to_string(int(*expiry_time)),
                              "Call",
                              std::to_string(
                                      flow::model::bsm::calculate_value(true, spot, strike, expiry_time_, cost_of_carry,
                                                                        risk_free, sigma)),
                              std::to_string(
                                      flow::model::crr::calculate_value(true, spot, strike, expiry_time_, cost_of_carry,
                                                                        risk_free, sigma, 16384)),
                              std::to_string(
                                      flow::model::baw::calculate_value(true, spot, strike, expiry_time_, cost_of_carry,
                                                                        risk_free, sigma)),
                              std::to_string(flow::model::juzhong::calculate_value(true, spot, strike, expiry_time_,
                                                                                   cost_of_carry, risk_free, sigma)),
                              std::to_string(flow::model::bjerk93::calculate_value(true, spot, strike, expiry_time_,
                                                                                   cost_of_carry, risk_free, sigma)),
                              std::to_string(flow::model::bjerk02::calculate_value(true, spot, strike, expiry_time_,
                                                                                   cost_of_carry, risk_free, sigma))
                      });
    }

    for (expiry_time = T.begin(); expiry_time != T.end(); expiry_time++) {
        double expiry_time_ = *expiry_time / 244.;

        table.add_row({
                              std::to_string(int(*expiry_time)),
                              "Put",
                              std::to_string(flow::model::bsm::calculate_value(false, spot, strike, expiry_time_,
                                                                               cost_of_carry, risk_free, sigma)),
                              std::to_string(flow::model::crr::calculate_value(false, spot, strike, expiry_time_,
                                                                               cost_of_carry, risk_free, sigma, 16384)),
                              std::to_string(flow::model::baw::calculate_value(false, spot, strike, expiry_time_,
                                                                               cost_of_carry, risk_free, sigma)),
                              std::to_string(flow::model::juzhong::calculate_value(false, spot, strike, expiry_time_,
                                                                                   cost_of_carry, risk_free, sigma)),
                              std::to_string(flow::model::bjerk93::calculate_value(false, spot, strike, expiry_time_,
                                                                                   cost_of_carry, risk_free, sigma)),
                              std::to_string(flow::model::bjerk02::calculate_value(false, spot, strike, expiry_time_,
                                                                                   cost_of_carry, risk_free, sigma))
                      });

    }
    std::cout << table << std::endl;
}


int main() {
    show();
    return 0;
}