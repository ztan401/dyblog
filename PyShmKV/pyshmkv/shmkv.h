//
// Created by 稻草人 on 2022/1/1.
//

#ifndef SHMKV_SHMKV_H
#define SHMKV_SHMKV_H

#include "skv_config.h"
#include "skv_shm.h"
#include "skv_store.h"
#include "spdlog/spdlog.h"
#include <string>

namespace ots::shmkv {
    inline int skv_init(skv_ctx *ctx,
                        char *path,
                        const unsigned int key_count,
                        const unsigned int value_size,
                        int op_type) {
        int ret;
        skv_store_hdr store_hdr;

        if (ctx->init) {
            SPDLOG_INFO("This library has been initialized.");
            return SKV_ERROR;
        }

        if (op_type == SKV_OP_TYPE_RW) {
            ctx->shm.size = skv_store_init(&store_hdr, key_count + 32, value_size * 3);
        }

        ctx->shm.path = strdup(path);

        ret = skv_shm_init(ctx, op_type);
        if (ret == SKV_ERROR) {
            return ret;
        }

        ctx->store_hdr = (skv_store_hdr *) ctx->shm.data;

        if (!ctx->shm.exists) {
            memcpy((char *) ctx->store_hdr, (char *) &store_hdr,
                   sizeof(skv_store_hdr));
        }

        ctx->store_items = (skv_store_item *) SKV_STORE_GET_ADDRESS(
                ctx->store_hdr, ctx->store_hdr->item_section);

        ctx->store_items_snapshot =
                (skv_store_item *) SKV_STORE_GET_SNAPSHOT_ADDRESS(
                        ctx->store_hdr, ctx->store_hdr->item_section);

        ctx->init = 1;

        // todo:搞清楚在不同系统和硬件平台限制大小的最大范围，这里限制mmap容量最大1GB
        if (ctx->shm.size > 1073741824) {
            return SKV_OVERFLOW;
        }
        // todo:这里限制最多有64*1024个key
        if (ctx->store_hdr->item_section.size > 65536) {
            return SKV_OVERFLOW;
        }

        return SKV_OK;
    }

    inline int skv_get(skv_ctx *ctx,
                       skv_str *key,
                       skv_str *value,
                       void *cache_ptr) {
        int ret;

        ret = skv_store_get(ctx, key, value, cache_ptr);
        if (ret != SKV_OK) {
            return ret;
        }

        return SKV_OK;
    }

    inline int skv_set(skv_ctx *ctx,
                       skv_str *key,
                       skv_str *value) {
        int ret;

        ret = skv_store_set(ctx, key, value);
        if (ret != SKV_OK) {
            return ret;
        }

        return SKV_OK;
    }

    inline int skv_del(skv_ctx *ctx,
                       skv_str *key) {
        int ret;

        ret = skv_store_del(ctx, key);
        if (ret != SKV_OK) {
            return ret;
        }

        return SKV_OK;
    }

    inline int skv_stats(skv_ctx *ctx,
                         skv_str *stat_json) {
        return skv_store_stats(ctx, stat_json);
    }

    inline int skv_finish(skv_ctx *skv_ctx) {
        int ret;

        if (skv_ctx->init) {
            ret = skv_store_finish(skv_ctx);
            if (ret != SKV_OK) {
                return ret;
            }
            skv_ctx->init = 0;
        }

        return SKV_OK;
    }
}// namespace ots::shmkv

#endif//SHMKV_SHMKV_H
