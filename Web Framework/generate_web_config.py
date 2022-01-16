#!/usr/bin/env python
# -*- coding: utf-8 -*-
# @Time    : 2020/8/9 12:45
# @Author  : dy
# @contact : aidabloc@163.com
# @File    : generate_web_config.py
# @Desc    :

import json
import logging
import os

# path = os.path.abspath(os.path.dirname(os.getcwd()))
path = os.getcwd() + os.sep + r"web-client" + os.sep + "src" + os.sep + "config" + os.sep + "left_menu.json"
logging.info(path)

# 配置表格
config = dict()
config["data"] = []
config["meta"] = {"msg": "获取左侧列表成功", "status": 200}

"""
动量
"""
# 动量 一级菜单
add_dict = dict()
add_dict["id"] = 101
add_dict["authName"] = "一级目录"
add_dict["level"] = 0
add_dict["path"] = "null"
add_dict["children"] = []

# 动量 二级菜单 每日收益统计表格
children_dict = dict()
children_dict["id"] = 201
children_dict["authName"] = "二级目录"
children_dict["level"] = 0
children_dict["pid"] = 0
children_dict["path"] = "test"
tmp_list = add_dict["children"]
tmp_list.append(children_dict)
add_dict["children"] = tmp_list

# 动量 一级菜单 更新
tmp_list = config["data"]
tmp_list.append(add_dict)
config["data"] = tmp_list
logging.info(config)

with open(path, "w") as fp:
    json.dump(config, fp)
