#include "nanotimestamp.h"
#include <pybind11/pybind11.h>
namespace py = pybind11;

int64_t now_in_nano() {
    return kungfu::yijinjing::time::now_in_nano();
}

int64_t time2num(const std::string &time) {
    return kungfu::yijinjing::time::strptime(time);
}

std::string num2time(const int64_t &time) {
    return kungfu::yijinjing::time::strftime(time);
}


PYBIND11_MODULE(pynanotime, m) {
    m.doc() = "nano timestamp";// optional module docstring
    m.def("now_in_nano", &now_in_nano, "now_in_nano");
    m.def("time2num", &time2num, "time2num");
    m.def("num2time", &num2time, "num2time");
}
