#include "logger.h"
#include "interface.h"
#include <iostream>
#include <string>

int test_set() {
    std::cout << "Hello, World!" << std::endl;
    const std::string path = "ipc.store";
    const unsigned int count = 32;
    const unsigned int value_size = 1024;
    const bool write_mode = true;
    const bool init_disk = true;
    const std::string mutex_name = "mutex_name";
    ots::shmkv::ShmKV shmkv(path, count, value_size, write_mode, init_disk, false, false, mutex_name);

    std::string key = "key_1";
    std::string value = "value_1";
    shmkv.set(key.c_str(), key.length(), value.c_str(), value.length());
    return 0;
}


int test_get() {
    std::cout << "Hello, World!" << std::endl;
    const std::string path = "ipc.store";
    const unsigned int count = 32;
    const unsigned int value_size = 1024;
    const bool write_mode = true;
    const bool init_disk = true;
    const std::string mutex_name = "mutex_name";
    ots::shmkv::ShmKV shmkv(path, count, value_size, write_mode, init_disk, false, false, mutex_name);

    std::string key = "key_1";
    std::string value = "value_1";
    shmkv.set(key.c_str(), key.length(), value.c_str(), value.length());

    ots::shmkv::skv_str skv_key, skv_value;
    skv_key.data = key.data();
    skv_key.len = std::size(key);
    shmkv.get(key.c_str(), key.length(), &skv_value);
    printf("get key:%s, value:%s, length:%d; \n", key.c_str(), skv_value.data, skv_value.len);
    return 0;
}


int test_del() {
    std::cout << "Hello, World!" << std::endl;
    const std::string path = "ipc.store";
    const unsigned int count = 32;
    const unsigned int value_size = 1024;
    const bool write_mode = true;
    const bool init_disk = true;
    const std::string mutex_name = "mutex_name";
    ots::shmkv::ShmKV shmkv(path, count, value_size, write_mode, init_disk, false, false, mutex_name);

    std::string key = "key_1";
    std::string value = "value_1";
    shmkv.set(key.c_str(), key.length(), value.c_str(), value.length());
    shmkv.del(key.c_str(), key.length());

    ots::shmkv::skv_str skv_key, skv_value;
    skv_key.data = key.data();
    skv_key.len = std::size(key);
    shmkv.get(key.c_str(), key.length(), &skv_value);
    printf("get key:%s, value:%s, length:%d; \n", key.c_str(), skv_value.data, skv_value.len);
    return 0;
}

int main() {
    std::cout << "Hello, World!" << std::endl;
    create_logger("clogs/shm.log", "trace", false, true, false, 1, 1);
    test_set();
    test_get();
    test_del();
}