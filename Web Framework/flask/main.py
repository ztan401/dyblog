#!/usr/bin/env python
# -*- coding: utf-8 -*-
# @Time    : 2022/1/6 15:01
# @Author  : dy
# @contact : aidabloc@163.com
# @File    : main.py
# @Desc    : Web后端服务器

from logging import info
from multiprocessing import cpu_count

from flask import Flask
from flask_cors import CORS

from logger import init_logger

# configuration
# DEBUG = True
NUMEXPR_MAX_THREADS = min(cpu_count(), 4)
# instantiate the app
app = Flask(__name__)
app.config.from_object(__name__)

# enable CORS
CORS(app, resources={r'/*': {'origins': '*'}})


@app.route("/")
def login():
    info("login!")
    return "hello world!"


if __name__ == "__main__":
    init_logger(file_name="web_backend_flask.log")
    NUMEXPR_MAX_THREADS = min(cpu_count(), 4)
    app.debug = True
    app.run(host="0.0.0.0", port=3321)
