//
// Created by 稻草人 on 2022/1/1.
//

#ifndef SHMKV_SKV_INTERFACE_H
#define SHMKV_SKV_INTERFACE_H

#include "process_mutex.h"
#include "shmkv.h"
#include <mutex>
#include <thread>


namespace ots::shmkv {
    class ShmKV {
    private:
        skv_ctx ctx{};
        CProcessMutex *ptr_process_mutex = nullptr;
        std::mutex ptr_thread_lock;
        void *cache_ptr = nullptr;

    private:
        static void init_disk_file(const std::string &path,
                                   const bool &write_mode) {
            if (write_mode) {
                if (remove(const_cast<char *>(path.c_str())) == 0)
                    SPDLOG_INFO("Removed {} succeeded.", const_cast<char *>(path.c_str()));
                else
                    SPDLOG_INFO("Removed {} failed.", const_cast<char *>(path.c_str()));
            }
        }

        int init_mmap(const std::string &path,
                      const unsigned int &key_count,
                      const unsigned int &value_size,
                      const bool &write_mode) {
            int ret;
            if (write_mode) {
                ret = skv_init(&ctx, const_cast<char *>(path.c_str()), key_count, value_size, SKV_OP_TYPE_RW);
            } else {
                ret = skv_init(&ctx, const_cast<char *>(path.c_str()), key_count, value_size, SKV_OP_TYPE_RO);
            }

            if (ret != SKV_OK) {
                SPDLOG_ERROR("Failed to initialize skv.");
                return ret;
            }
            return 0;
        }

    public:
        ShmKV(const std::string &path,
              const unsigned int &count,
              const unsigned int &value_size,
              const bool &write_mode,
              const bool &init_disk,
              const bool &thread_lock = false,
              const bool &process_lock = false,
              const std::string &process_mutex = "process_mutex") {
            init_disk_file(path, init_disk);
            int ret = init_mmap(path, count, value_size, write_mode);
            if (ret != 0) {
                throw std::runtime_error("SKV初始化失败！");
            }

            // get memory
            cache_ptr = malloc(count * sizeof(skv_store_item) + 1);

            if (process_lock & thread_lock) {
                SPDLOG_ERROR("error config, process_lock true, thread_lock true!");
                exit(SKV_ERROR);
            }

            if (process_lock) {
                ptr_process_mutex = new CProcessMutex(process_mutex.c_str());
            }
        }

        ~ShmKV() {
            skv_finish(&ctx);
            delete ptr_process_mutex;
            free(cache_ptr);
            cache_ptr = nullptr;
        }

    public:
        bool set(const char *key,
                 const unsigned int &key_len,
                 const char *value,
                 const unsigned int &value_len) {
            skv_str skv_key, skv_value;
            skv_key.len = key_len;
            skv_key.data = const_cast<char *>(key);
            skv_value.len = value_len;
            skv_value.data = const_cast<char *>(value);

            int ret = skv_set(&ctx, &skv_key, &skv_value);
            if (ret != SKV_OK) {
                SPDLOG_ERROR("Failed to set {}.", skv_key.data);
                return true;
            }
            return false;
        }

        bool set_process_mutex(const char *key,
                               const unsigned int &key_len,
                               const char *value,
                               const unsigned int &value_len) {
            bool lock = ptr_process_mutex->Lock();
            if (lock) {
                bool ret = set(key, key_len, value, value_len);
                lock = ptr_process_mutex->UnLock();
                return ret;
            } else {
                return false;
            }
        }

        bool set_thread_mutex(const char *key,
                              const unsigned int &key_len,
                              const char *value,
                              const unsigned int &value_len) {
            ptr_thread_lock.lock();
            bool ret = set(key, key_len, value, value_len);
            ptr_thread_lock.unlock();
            return ret;
        }

        bool get(const char *key,
                 const unsigned int &key_len,
                 skv_str *skv_value) {
            skv_str skv_key;
            skv_key.len = key_len;
            skv_key.data = const_cast<char *>(key);

            int ret = skv_get(&ctx, &skv_key, skv_value, cache_ptr);
            if (ret != SKV_OK) {
                SPDLOG_ERROR("Failed to get {}", skv_key.data);
                return true;
            }
            return false;
        }

        bool del(const char *key,
                 const unsigned int &key_len) {
            skv_str skv_key;
            skv_key.len = key_len;
            skv_key.data = const_cast<char *>(key);

            int ret = skv_del(&ctx, &skv_key);
            if (ret != SKV_OK) {
                return true;
            }
            return false;
        }
    };


}// namespace ots::shmkv
#endif//SHMKV_SKV_INTERFACE_H
