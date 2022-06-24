//
// Created by 稻草人 on 2022/1/6.
//
// malloc/free只是动态分配内存空间/释放空间。而new/delete除了分配空间还会调用构造函数和析构函数进行初始化与清理（清理成员）。

#include "logger.h"
#include "interface.h"
#include <memory>
#include <pybind11/pybind11.h>

namespace py = pybind11;


class PyShmKV {
private:
    ots::shmkv::ShmKV *p;

public:
    PyShmKV(const std::string &path,
            const unsigned int &count,
            const unsigned int &value_size,
            const bool &write_mode,
            const bool &init_disk,
            const bool &thread_lock = false,
            const bool &process_lock = false,
            const std::string &process_mutex = "process_mutex") {
        auto ret = create_logger("clogs/shmkv.log", "info", false, false, true, 1, 1);
        p = new ots::shmkv::ShmKV(path, count, value_size, write_mode, init_disk, thread_lock, process_lock, process_mutex);
    }

    ~PyShmKV() {
        delete p;
    }

    bool set(const std::string &k,
             const std::string &v,
             const unsigned int &value_len) {
        bool ret = p->set(const_cast<char *>(k.c_str()),
                          static_cast<int>(k.length()),
                          const_cast<char *>(v.c_str()),
                          value_len);
        return ret;
    };

    bool set_process_mutex(const std::string &k,
                           const std::string &v,
                           const unsigned int &value_len) {
        bool ret = p->set_process_mutex(const_cast<char *>(k.c_str()),
                                        static_cast<int>(k.length()),
                                        const_cast<char *>(v.c_str()),
                                        value_len);
        return ret;
    };

    bool set_thread_mutex(const std::string &k,
                          const std::string &v,
                          const unsigned int &value_len) {
        bool ret = p->set_thread_mutex(const_cast<char *>(k.c_str()),
                                       static_cast<int>(k.length()),
                                       const_cast<char *>(v.c_str()),
                                       value_len);
        return ret;
    };

    py::tuple get(const std::string &k) {
        ots::shmkv::skv_str skv_value;
        bool ret = p->get(const_cast<char *>(k.c_str()),
                          static_cast<int>(k.length()),
                          &skv_value);
        if (ret) {
            return py::make_tuple(true, "null");
        }
        std::string str = skv_value.data;
        return py::make_tuple(false, str);
    }

    bool del(const std::string &k) {
        bool ret = p->del(const_cast<char *>(k.c_str()),
                          static_cast<int>(k.length()));
        return ret;
    };
};

PYBIND11_MODULE(pyshmkv, m) {
    py::class_<PyShmKV>(m, "PyShmKV")
            .def(py::init<const std::string &,
                          const unsigned int &,
                          const unsigned int &,
                          const bool &,
                          const bool &,
                          const bool &,
                          const bool &,
                          const std::string &>())
            .def("set", &PyShmKV::set)
            .def("set_process_mutex", &PyShmKV::set_process_mutex)
            .def("set_thread_mutex", &PyShmKV::set_thread_mutex)
            .def("get", &PyShmKV::get)
            .def("del_void", &PyShmKV::del);
}
