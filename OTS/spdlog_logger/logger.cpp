//
// Created by 观鱼 on 2020/6/13.
//
#include <iostream>
#include <string>
#include <memory>
#include "logger.h"


namespace utils::log {
    bool init_console_logger(const std::string &level,
                             bool show_use_level,
                             bool use_format) {
        try {
            // console
            if (use_format) {
                spdlog::set_pattern(SPDLOG_LOG_PATTERN);
            }

            if (level == "trace") {
                spdlog::set_level(spdlog::level::trace); // Set global log level to debug
            } else if (level == "debug") {
                spdlog::set_level(spdlog::level::debug);
            } else if (level == "info") {
                spdlog::set_level(spdlog::level::info);
            } else if (level == "warn") {
                spdlog::set_level(spdlog::level::warn);
            } else if (level == "error") {
                spdlog::set_level(spdlog::level::err);
            } else if (level == "critical") {
                spdlog::set_level(spdlog::level::critical);
            } else if (level == "off") {
                spdlog::set_level(spdlog::level::off);
            } else {
                std::cout << "level should be in [trace, debug, info, warn, error, critical, off]!";
                spdlog::set_level(spdlog::level::trace);
            }

            if (show_use_level) {
                SPDLOG_TRACE("Trace from SpdLog!");
                SPDLOG_DEBUG("Debug from SpdLog!");
                SPDLOG_INFO("Info from SpdLog!");
                SPDLOG_WARN("Warn from SpdLog!");
                SPDLOG_ERROR("Error from SpdLog!");
                SPDLOG_CRITICAL("Critical from SpdLog!");
            }
            return true;

        }
        catch (const spdlog::spdlog_ex &ex) {
            std::cout << "Log initialization failed: " << ex.what() << std::endl;
            std::cout << ex.what() << std::endl;
            return false;
        }
    }


    bool init_logger(const std::string &level,
                     const std::string &log_file,
                     bool use_async,
                     bool show_use_level,
                     bool use_format,
                     bool only_console) {
        if (only_console) {
            return init_console_logger(level, show_use_level, use_format);
        }

        try {
            // console
            auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
            // Create a daily logger - a new file is created every day on 4:00am
            auto daily_sink = std::make_shared<spdlog::sinks::daily_file_sink_mt>(log_file, 4, 00);

            if (use_async) {
                spdlog::init_thread_pool(8192, 1);
                std::vector<spdlog::sink_ptr> log_sinks{console_sink, daily_sink};
                auto logger = std::make_shared<spdlog::async_logger>("logger", log_sinks.begin(), log_sinks.end(),
                                                                     spdlog::thread_pool(),
                                                                     spdlog::async_overflow_policy::block);
                if (use_format) {
                    logger->set_pattern(SPDLOG_LOG_PATTERN);
                }
                spdlog::set_default_logger(logger);
            } else {
                spdlog::sinks_init_list log_sinks = {console_sink, daily_sink};
                auto logger = std::make_shared<spdlog::logger>("logger", log_sinks);
                if (use_format) {
                    logger->set_pattern(SPDLOG_LOG_PATTERN);
                }
                spdlog::set_default_logger(logger);
            }

            if (level == "trace") {
                spdlog::set_level(spdlog::level::trace); // Set global log level to debug
            } else if (level == "debug") {
                spdlog::set_level(spdlog::level::debug);
            } else if (level == "info") {
                spdlog::set_level(spdlog::level::info);
            } else if (level == "warn") {
                spdlog::set_level(spdlog::level::warn);
            } else if (level == "error") {
                spdlog::set_level(spdlog::level::err);
            } else if (level == "critical") {
                spdlog::set_level(spdlog::level::critical);
            } else if (level == "off") {
                spdlog::set_level(spdlog::level::off);
            } else {
                std::cout << "level should be in [trace, debug, info, warn, error, critical, off]!";
                spdlog::set_level(spdlog::level::trace);
            }

            if (show_use_level) {
                SPDLOG_TRACE("Trace from SpdLog!");
                SPDLOG_DEBUG("Debug from SpdLog!");
                SPDLOG_INFO("Info from SpdLog!");
                SPDLOG_WARN("Warn from SpdLog!");
                SPDLOG_ERROR("Error from SpdLog!");
                SPDLOG_CRITICAL("Critical from SpdLog!");
            }

            return true;
        }
        catch (const spdlog::spdlog_ex &ex) {
            std::cout << "Log initialization failed: " << ex.what() << std::endl;
            std::cout << ex.what() << std::endl;
            return false;
        }
    }
}

