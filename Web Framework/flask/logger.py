#!/usr/bin/env python
# -*- coding: utf-8 -*-
# @Time    : 2020/6/21 11:01
# @Author  : dy
# @contact : aidabloc@163.com
# @File    : logger.py
# @Desc    :

import logging
import os

from colorlog import ColoredFormatter


# 获取存放临时文件的路径
def get_log_path_filename(file_name: str = "log.log", dir_name: str = "logfile") -> str:
    # 调整编码格式为GBK
    file_name = file_name.encode("GBK").decode("GBK")
    dir_name = dir_name.encode("GBK").decode("GBK")
    # path = os.getcwd().decode("GBK")
    # # 适配python3
    path = os.getcwd()
    log_path = os.path.join(path, dir_name)

    if not os.path.exists(log_path):
        os.makedirs(log_path)
    return os.path.join(log_path, file_name)


def init_logger(file_name: str = "test.log", file_path: str = "logfile", level: str = "debug", fmt: str = None,
                show_init: bool = True):
    """

    :param file_name:
    :param file_path:
    :param level:
    :param fmt:
    :param show_init:
    :return:
    """
    file_name = get_log_path_filename(file_name, file_path)

    if level == "debug":
        level = logging.DEBUG
    elif level == "info":
        level = logging.INFO
    elif level == "warn" or level == "warning":
        level = logging.WARN
    elif level == "error":
        level = logging.ERROR
    elif level == "critical" or level == "cri":
        level = logging.CRITICAL
    else:
        raise ValueError("level list [debug, info, warn, error, critical]")

    if fmt is None:
        # fmt_file = u"[%(asctime)s][%(filename)s:%(lineno)d][%(levelname)s][%(thread)d] - %(message)s"
        fmt_file = u'[%(asctime)s] [%(levelname)s] [%(process)6d-%(thread)6d] [%(pathname)s:%(lineno)d#%(funcName)s] %(message)s '
    else:
        fmt_file = fmt

    # Configure the logging system
    logging.basicConfig(
        filename=file_name,
        level=level,
        format=fmt_file
    )

    if fmt is None:
        # fmt = u"[%(asctime)s][%(filename)s:%(lineno)d][%(levelname)s][%(thread)d] - %(message)s"
        # fmt_console = u'%(log_color)s [%(asctime)s] [%(levelname)s] [%(process)6d-%(thread)6d] %(reset)s [%(pathname)s:%(lineno)d#%(funcName)s] %(message)s '
        fmt_console = u'%(log_color)s[%(asctime)s][%(levelname)s][%(process)6d-%(thread)6d][%(pathname)s:%(lineno)d#%(funcName)s] %(message)s %(reset)s'
    else:
        fmt_console = fmt

    formatter = ColoredFormatter(
        fmt=fmt_console,
        datefmt="%H:%M:%S",
        reset=True,
        log_colors={
            'DEBUG': 'cyan',
            'INFO': 'green',
            'WARNING': 'bold_yellow',
            'ERROR': 'bold_red',
            'CRITICAL': 'bold_red,bg_white',
        })

    # 设置屏幕上显示的格式
    console = logging.StreamHandler()
    # format_str = logging.Formatter(fmt_console)
    # console.setFormatter(format_str)
    console.setFormatter(formatter)
    console.setLevel(level)
    logging.getLogger("").addHandler(console)

    if show_init:
        logging.debug("logger module init!")
        logging.info("logger module init!")
        logging.warning("logger module init!")
        logging.error("logger module init!")
        logging.critical("logger module init!")