//
// Created by 杨东 on 2020/6/13.
//

#ifndef DYLIB_LOGGER_H
#define DYLIB_LOGGER_H

#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE//必须定义这个宏,才能输出文件名和行号
#define SPDLOG_LOG_PATTERN "[%m/%d %T.%F] [%^%=8l%$] [%6P/%-6t] [%@#%!] %v"

#include "spdlog/async.h"
#include "spdlog/sinks/daily_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/spdlog.h"

namespace flow::utils {

    bool init_console_logger(const std::string &level = "trace", bool show_use_level = false, bool use_format = false);

    bool init_logger(const std::string &level = "trace",
                     const std::string &log_file = "clogs/daily.log",
                     bool use_async = false,
                     bool show_use_level = false,
                     bool use_format = false);
}// namespace flow::utils

#endif//DYLIB_LOGGER_H
