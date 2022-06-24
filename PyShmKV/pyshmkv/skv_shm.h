//
// Created by 稻草人 on 2021/12/28.
//

#ifndef PYSHMKV_SKV_SHM_H
#define PYSHMKV_SKV_SHM_H

#include "skv_config.h"
#include "spdlog/spdlog.h"
#include <cerrno>
#include <fcntl.h>
#include <sys/stat.h>

#ifdef _WIN32
#include "io.h"
#include "mman.h"
#else
#include <sys/mman.h>
#include <unistd.h>
#endif
#include "skv_data.h"

namespace ots::shmkv {
    // mmap 初始化
    int skv_shm_init(skv_ctx *ctx, int op_type) {
        int fd;
        int flag;
        skv_shm_store *shm;
        struct stat st {};

        shm = &ctx->shm;

        if (op_type == SKV_OP_TYPE_RO) {
            fd = open(shm->path, O_RDONLY);
            if (fd == SKV_ERROR) {
                SPDLOG_ERROR("Failed to open {}, errno: {}", shm->path, errno);
                return SKV_ERROR;
            }

            if (fstat(fd, &st) == SKV_ERROR) {
                SPDLOG_ERROR("Failed to fstat {}, errno: {}", shm->path, errno);
                close(fd);
                return SKV_ERROR;
            }

            if (st.st_size == 0) {
                SPDLOG_ERROR("File {} is empty.", shm->path);
                close(fd);
                return SKV_ERROR;
            }

            shm->size = st.st_size;
            shm->exists = 1;

            shm->data = mmap(nullptr, shm->size, PROT_READ, MAP_PRIVATE, fd, 0);
            if (shm->data == MAP_FAILED) {
                SPDLOG_ERROR("Failed to mmap {}, size: {}, errno: {}", shm->path,
                             shm->size, errno);
                close(fd);
                return SKV_ERROR;
            }
        } else {
            flag = O_CREAT | O_RDWR;

            fd = open(shm->path, flag, 0666);
            if (fd == SKV_ERROR) {
                SPDLOG_ERROR("Failed to open {}, errno: {}", shm->path, errno);
                return SKV_ERROR;
            }

            if (fstat(fd, &st) == SKV_ERROR) {
                SPDLOG_ERROR("Failed to fstat {}, errno: {}", shm->path, errno);
                close(fd);
                return SKV_ERROR;
            }

            shm->exists = (st.st_size == 0) ? 0 : 1;
            shm->size = (st.st_size == 0) ? shm->size : st.st_size;

            if (!shm->exists) {

#ifdef _WIN32
                if (_chsize(fd, (long) (shm->size)) == SKV_ERROR) {
#else
                if (ftruncate(fd, (off_t) shm->size) == SKV_ERROR) {
#endif
                    SPDLOG_ERROR("Failed to ftruncate {}, size: {}, errno: {}",
                                 shm->path, shm->size, errno);
                    return SKV_ERROR;
                }
            }

            shm->data =
                    mmap(nullptr, shm->size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
            if (shm->data == MAP_FAILED) {
                SPDLOG_ERROR("Failed to mmap {}, size: {}, errno: {}", shm->path,
                             shm->size, errno);
                close(fd);
                return SKV_ERROR;
            }
        }

        close(fd);

        return SKV_OK;
    }

    // 同步
    inline int skv_shm_sync(skv_ctx *ctx) {
        if (msync(ctx->store_hdr, ctx->shm.size, MS_SYNC) != 0) {
            return SKV_ERROR;
        }

        return SKV_OK;
    }


    inline int skv_shm_finish(skv_ctx *ctx) {
        if (munmap(ctx->store_hdr, ctx->shm.size) != SKV_OK) {
            return SKV_ERROR;
        }

        return SKV_OK;
    }

}// namespace ots::shmkv

#endif//PYSHMKV_SKV_SHM_H
