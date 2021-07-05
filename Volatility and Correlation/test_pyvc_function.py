#!/usr/bin/env python
# -*- coding: utf-8 -*-
# @Time    : 2021/7/4 18:04
# @Author  : dy
# @contact : aidabloc@163.com
# @File    : test_pyvc_function.py
# @Desc    :
import talib
from numpy import log, sqrt, double, isclose, ndarray, power
from pandas import read_excel
from pyflow.model import pyvc

data = read_excel("HS300.xlsx", index_col="Unnamed: 0")
open_s = data["open"].values[1:].astype(double)
high = data["high"].values[1:].astype(double)
low = data["low"].values[1:].astype(double)
close = data["close"].values[1:].astype(double)
pre_close = data["close"].values[:-1].astype(double)


def test_hv_cc(shift_num: int = 20):
    print("hv-cc")
    print(pyvc.calc_hv_cc(close, shift_num, sqrt(244))[-1])

    ret = log(close[-shift_num:] / close[-shift_num - 1:-1])
    print(sqrt((ret ** 2).sum() / shift_num) * sqrt(244))

    print(isclose(pyvc.calc_hv_cc(close, shift_num, sqrt(244))[-1], sqrt((ret ** 2).sum() / shift_num) * sqrt(244)))


def test_hv_ema(shift_num: int = 20):
    print("hv-ema")
    print(pyvc.calc_hv_cc_ema(close, shift_num)[-1])

    ret = log(close[1:] / close[:-1])
    var = ret ** 2
    print(sqrt(talib.EMA(var, shift_num)[-1]) * sqrt(244))
    print(isclose(pyvc.calc_hv_cc_ema(close, shift_num)[-1], sqrt(talib.EMA(var, shift_num)[-1]) * sqrt(244)))


def test_hv_pk(shift_num: int = 20):
    print("hv-pk")
    print(pyvc.calc_hv_pk(high, low, shift_num)[-1])

    pk = sqrt((log(high[-shift_num:] / low[-shift_num:]) ** 2).sum() / (4 * log(2) * 20)) * sqrt(244)
    print(pk)
    print(isclose(pyvc.calc_hv_pk(high, low, shift_num)[-1], pk))


def test_hv_gk(shift_num: int = 20):
    print("hv-gk")
    print(pyvc.calc_hv_gk(open_s, high, low, close, shift_num)[-1])
    gk = sqrt((0.5 * log(high[-shift_num:] / low[-shift_num:]) ** 2 - (2 * log(2) - 1) * log(
        close[-shift_num:] / open_s[-shift_num:]) ** 2).sum() / shift_num) * sqrt(244)
    print(gk)
    print(isclose(pyvc.calc_hv_gk(open_s, high, low, close, shift_num)[-1], gk))


def test_hv_rs(shift_num: int = 20):
    print("hv-rs")
    print(pyvc.calc_hv_rs(open_s, high, low, close, shift_num)[-1])
    rs = sqrt((log(high[-shift_num:] / close[-shift_num:]) * log(high[-shift_num:] / open_s[-shift_num:]) + log(
        low[-shift_num:] / close[-shift_num:]) * log(low[-shift_num:] / open_s[-shift_num:])).sum() / shift_num) * sqrt(
        244)
    print(rs)
    print(isclose(pyvc.calc_hv_rs(open_s, high, low, close, shift_num)[-1], rs))


def test_hv_gkyz(shift_num: int = 20):
    print("hv-gkyz")
    print(pyvc.calc_hv_gkyz(open_s, high, low, close, pre_close, shift_num)[-1])
    gkyz = sqrt((log(open_s[-shift_num:] / close[-shift_num - 1:-1]) ** 2 + 0.5 * log(
        high[-shift_num:] / low[-shift_num:]) ** 2 - (2 * log(2) - 1) * log(
        close[-shift_num:] / open_s[-shift_num:]) ** 2).sum() / shift_num) * sqrt(244)
    print(gkyz)
    print(isclose(pyvc.calc_hv_gkyz(open_s, high, low, close, pre_close, shift_num)[-1], gkyz))


def yang_zhang(open: ndarray, high: ndarray, low: ndarray, close: ndarray, pre_close: ndarray, size: int) -> double:
    """Yang-Zhang is the sum of overnight volatility, and a weighted average of Rogers-Satchell
    and open-to-close volatility.

    In 2000 Yang-Zhang created a volatility measure that handles both opening jumps and drift. It
    is the sum of the overnight volatility (close-to-open volatility) and a weighted average of the
    Rogers-Satchell volatility and the open-to-close volatility. The assumption of continuous prices
    does mean the measure tends to slightly underestimate the volatility.
    :param open:
    :param high:
    :param low:
    :param close:
    :param pre_close:
    :param size:
    :return:
    """
    k: double = double(0.34 / (1.34 + (size + 1) / (size - 1)))

    # overnight volatility
    loc: ndarray = log(open / pre_close)
    ov: double = power(loc - loc.mean(), 2).sum() / (size - 1)

    # open to close volatility
    lco: ndarray = log(close / open)
    oc: double = power(lco - lco.mean(), 2).sum() / (size - 1)

    #  Rogers-Satchell volatility
    rs: double = (log(high / close) * log(high / open) + log(low / close) * log(low / open)).sum() / size
    return sqrt(ov + k * oc + (1 - k) * rs)


def test_hv_yz(shift_num: int = 20):
    print("hv-yz")
    print(pyvc.calc_hv_yz(open_s, high, low, close, pre_close, shift_num)[-1])
    print(yang_zhang(open_s[-shift_num:], high[-shift_num:], low[-shift_num:], close[-shift_num:],
                     close[-shift_num - 1:-1], shift_num) * sqrt(244))
    print(isclose(pyvc.calc_hv_yz(open_s, high, low, close, pre_close, shift_num)[-1],
                  yang_zhang(open_s[-shift_num:], high[-shift_num:], low[-shift_num:], close[-shift_num:],
                             close[-shift_num - 1:-1], shift_num) * sqrt(244)))


if __name__ == "__main__":
    for shift_num in range(5, 300):
        print("shift num: {}".format(shift_num))
        test_hv_cc(shift_num)
        test_hv_ema(shift_num)
        test_hv_pk(shift_num)
        test_hv_gk(shift_num)
        test_hv_rs(shift_num)
        test_hv_gkyz(shift_num)
        test_hv_yz(shift_num)
        print("----------------------------")
