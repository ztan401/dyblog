#include "logger.h"
#include <iostream>

int main() {
    std::cout << "Hello, World!" << std::endl;

    utils::log::init_logger();
    SPDLOG_TRACE("Trace from SpdLog!");
    SPDLOG_DEBUG("Debug from SpdLog!");
    SPDLOG_INFO("Info from SpdLog!");
    SPDLOG_WARN("Warn from SpdLog!");
    SPDLOG_ERROR("Error from SpdLog!");
    SPDLOG_CRITICAL("Critical from SpdLog!");
    return 0;
}



//int main() {
//    std::cout << "Hello, World!" << std::endl;
//
//    flow::utils::init_logger();
//    SPDLOG_TRACE("Trace from SpdLog!");
//    SPDLOG_DEBUG("Debug from SpdLog!");
//    SPDLOG_INFO("Info from SpdLog!");
//    SPDLOG_WARN("Warn from SpdLog!");
//    SPDLOG_ERROR("Error from SpdLog!");
//    SPDLOG_CRITICAL("Critical from SpdLog!");
//    return 0;
//}
