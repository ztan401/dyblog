//
// Created by 稻草人 on 2021/12/28.
//

#ifndef PYSHMKV_SKV_STORE_H
#define PYSHMKV_SKV_STORE_H


#include "skv_data.h"
#include <cstddef>
#include <cstdlib>
#include <cstring>
namespace ots::shmkv {

    // 初始化内存空间
    inline size_t skv_store_init(
            skv_store_hdr *store_hdr,
            unsigned int key_count,
            unsigned int value_size) {
        size_t offset = sizeof(skv_store_hdr);

        //  header memory management 初始化
        //  initialize cache item section
        store_hdr->item_section.size = key_count;
        store_hdr->item_section.used = 0;
        store_hdr->item_section.id = SKV_STORE_SECTION_ONE;
        store_hdr->item_section.sec1_offset = offset;
        offset += key_count * (long) sizeof(skv_store_item);
        store_hdr->item_section.sec2_offset = offset;
        offset += key_count * (long) sizeof(skv_store_item);

        //  initialize cache key section
        store_hdr->key_section.size = SKV_KEY_AVERAGE_LEN * key_count;
        store_hdr->key_section.used = 0;
        store_hdr->key_section.id = SKV_STORE_SECTION_ONE;
        store_hdr->key_section.sec1_offset = offset;
        offset += key_count * SKV_KEY_AVERAGE_LEN;
        store_hdr->key_section.sec2_offset = offset;
        offset += key_count * SKV_KEY_AVERAGE_LEN;

        //  initialize cache value section
        store_hdr->value_section.size = value_size * key_count;
        store_hdr->value_section.used = 0;
        store_hdr->value_section.id = SKV_STORE_SECTION_ONE;
        store_hdr->value_section.sec1_offset = offset;
        offset += key_count * value_size;
        store_hdr->value_section.sec2_offset = offset;
        offset += key_count * value_size;

        return offset;
    }


    inline int skv_store_item_compare_(
            const void *p1,
            const void *p2,
            void *arg) {
        int ret;
        char *store_hdr = nullptr;
        skv_store_item_wrapper *store_item_wrapper = nullptr;

        ret = (int) ((skv_store_item *) p1)->key_len - (int) ((skv_store_item *) p2)->key_len;
        if (ret != 0) {
            return ret;
        }

        store_item_wrapper = (skv_store_item_wrapper *) arg;
        store_hdr = (char *) store_item_wrapper->ctx->store_hdr;
        if (store_item_wrapper->key_offset == 0) {
            ret = memcmp(store_hdr + ((skv_store_item *) p1)->key_offset,
                         store_hdr + ((skv_store_item *) p2)->key_offset,
                         ((skv_store_item *) p1)->key_len);
        } else {
            ret = memcmp(store_hdr + store_item_wrapper->key_offset,
                         store_hdr + ((skv_store_item *) p2)->key_offset,
                         ((skv_store_item *) p1)->key_len);
        }
        return ret;
    }

    inline int skv_store_item_key_compare(const void *p1,
                                          const void *p2) {
        skv_store_item_wrapper *p_store_item_wrapper = nullptr;
        // todo： 这里通过偏移量的方式，通过p1获取结构体指针skv_store_item_wrapper的首位置
        // p1 is the target value, P2 is each value in the array.
        p_store_item_wrapper = (skv_store_item_wrapper *) ((char *) p1 - offsetof(skv_store_item_wrapper, store_item));

        return skv_store_item_compare_((void *) p1, p2, p_store_item_wrapper);
    }

    inline int skv_store_get(skv_ctx *ctx,
                             skv_str *key,
                             skv_str *value,
                             void *cache_ptr) {
        skv_store_item *p_cache_item = nullptr;
        skv_store_item *p_store_item = nullptr;
        skv_store_section *p_item_section = nullptr;
        skv_store_item_wrapper store_item_wrapper;

        // 获取当前读取操作正在使用的buffer（buffer-1或buffer-2）的内存首位置
        p_store_item = (skv_store_item *) SKV_STORE_GET_ADDRESS(
                ctx->store_hdr, ctx->store_hdr->item_section);

        // 创建item的副本，对超大容量保证 get id0 then set change id 0->1 change item values
        memcpy(cache_ptr, p_store_item, sizeof(skv_store_item) * ctx->store_hdr->item_section.used);


        //  todo:操作共享内存外的数据，以共享内存为首位置坐标计算偏移量?
        store_item_wrapper.ctx = ctx;
        store_item_wrapper.key_offset = (key->data - (char *) ctx->store_hdr);
        store_item_wrapper.store_item.key_len = key->len + 1;

        p_item_section = &ctx->store_hdr->item_section;
        p_cache_item = static_cast<skv_store_item *>(bsearch(&store_item_wrapper.store_item,
                                                             cache_ptr,
                                                             p_item_section->used,
                                                             sizeof(skv_store_item),
                                                             skv_store_item_key_compare));

        if (p_cache_item == nullptr) {
            SPDLOG_TRACE("Not found key: {}", key->data);
            return SKV_NOT_FOUND;
        }

        if (p_cache_item->key_len == 0) {
            SPDLOG_ERROR("The length of the key {} is invalid.", key->data);
            return SKV_ERROR;
        }

        value->data = (char *) ctx->store_hdr + p_cache_item->value_offset;
        value->len = p_cache_item->value_len;
        return SKV_OK;
    }

    //双buffer 创建快照
    inline void skv_store_item_make_snapshot(skv_ctx *ctx) {
        skv_store_item *p_cache_items = nullptr;
        skv_store_item *p_store_items = nullptr;

        p_cache_items = (skv_store_item *) SKV_STORE_GET_SNAPSHOT_ADDRESS(
                ctx->store_hdr, ctx->store_hdr->item_section);

        p_store_items = (skv_store_item *) SKV_STORE_GET_ADDRESS(
                ctx->store_hdr, ctx->store_hdr->item_section);

        if (ctx->store_hdr->item_section.used) {
            memcpy(p_cache_items, p_store_items,
                   sizeof(skv_store_item) * ctx->store_hdr->item_section.used);
        }
        ctx->store_items_snapshot = p_cache_items;
    }


    // 注意这里重新构建snapshot了，从store重新构建snapshot
    // 构建完成之后，切换snapshot和store，原子操作
    inline int skv_store_key_compaction(skv_ctx *ctx,
                                        skv_str *key,
                                        char **address) {
        int i;
        char *p = nullptr;
        size_t used_size;
        size_t offset;
        skv_store_item *p_cache_item = nullptr;

        p = SKV_STORE_GET_SNAPSHOT_ADDRESS(ctx->store_hdr, ctx->store_hdr->key_section);
        offset = SKV_STORE_GET_SNAPSHOT_OFFSET(ctx->store_hdr->key_section);

        used_size = 0;
        for (i = 0; i < ctx->store_hdr->item_section.used; i++) {
            p_cache_item = &ctx->store_items_snapshot[i];
            memcpy(p + used_size,
                   (char *) ctx->store_hdr + p_cache_item->key_offset,
                   p_cache_item->key_len);
            p_cache_item->key_offset = offset + used_size;
            used_size += p_cache_item->key_len;
        }

        if ((key->len + 1 + used_size) > ctx->store_hdr->key_section.size) {
            return SKV_OVERFLOW;
        }

        *address = p;
        ctx->store_hdr->key_section.used = used_size;
        ctx->store_hdr->key_section.id = SKV_STORE_GET_SEC_SNAPSHOT_ID(
                ctx->store_hdr->key_section);

        return SKV_OK;
    }

    inline int skv_store_value_compaction(skv_ctx *ctx,
                                          skv_str *value,
                                          char **address) {
        int i;
        char *p = nullptr;
        size_t used_size;
        size_t offset;
        skv_store_item *p_cache_item;

        p = SKV_STORE_GET_SNAPSHOT_ADDRESS(ctx->store_hdr,
                                           ctx->store_hdr->value_section);

        offset = SKV_STORE_GET_SNAPSHOT_OFFSET(ctx->store_hdr->value_section);

        used_size = 0;
        for (i = 0; i < ctx->store_hdr->item_section.used; i++) {
            p_cache_item = &ctx->store_items_snapshot[i];
            memcpy(p + used_size,
                   (char *) ctx->store_hdr + p_cache_item->value_offset,
                   p_cache_item->value_len);
            p_cache_item->value_offset = offset + used_size;
            used_size += p_cache_item->value_len;
        }

        if ((value->len + 1 + used_size) > ctx->store_hdr->value_section.size) {
            return SKV_OVERFLOW;
        }

        *address = p + used_size;
        ctx->store_hdr->value_section.used = used_size;
        ctx->store_hdr->value_section.id =
                SKV_STORE_GET_SEC_SNAPSHOT_ID(ctx->store_hdr->value_section);

        return SKV_OK;
    }

    inline int skv_store_item_add(skv_ctx *ctx,
                                  skv_store_item *store_item,
                                  skv_str *key,
                                  skv_str *value) {
        int ret;
        size_t offset;
        char *p = nullptr;
        skv_store_hdr *store_hdr = ctx->store_hdr;

        if (store_hdr->item_section.used == store_hdr->item_section.size) {
            return SKV_OVERFLOW;
        }

        //* 如果发现key data section空间内还有足够的剩余空间，直接在store最后位置添加新增的key数据，char格式copy进去。
        // 因为这个时候没有增加item里面used数据，所以在get操作里面是看不到这个在末尾新增的数据块的。
        //* 如果发现key data section空间内不够，重新创建一个snapshot（减少失去item指向的无效空间），然后将snapshot与store互换之后重复上一步操作。
        if ((store_hdr->key_section.used + (key->len + 1)) >
            store_hdr->key_section.size) {
            SPDLOG_INFO("Start to compact the key section.");

            offset = SKV_STORE_GET_SNAPSHOT_OFFSET(store_hdr->key_section);
            ret = skv_store_key_compaction(ctx, key, &p);
            if (ret != SKV_OK) {
                return ret;
            }
        } else {
            p = SKV_STORE_GET_FREE_ADDRESS(store_hdr, store_hdr->key_section);
            offset = SKV_STORE_GET_OFFSET(store_hdr->key_section);
        }

        //  copy the data of the key to the cache
        store_item->key_len = key->len + 1;
        store_item->key_offset = offset + store_hdr->key_section.used;

        // 字符串末尾添加 \0结尾
        memcpy(p, key->data, key->len);
        p += key->len;
        *p = '\0';
        store_hdr->key_section.used += store_item->key_len;

        //  copy the data of the value to the cache
        p = nullptr;

        if ((store_hdr->value_section.used + (value->len + 1)) >
            store_hdr->value_section.size) {
            SPDLOG_TRACE("Start to compact the value section.");

            offset = SKV_STORE_GET_SNAPSHOT_OFFSET(store_hdr->value_section);

            ret = skv_store_value_compaction(ctx, value, &p);
            if (ret != SKV_OK) {
                return ret;
            }
        } else {
            p = SKV_STORE_GET_FREE_ADDRESS(store_hdr, store_hdr->value_section);
            offset = SKV_STORE_GET_OFFSET(store_hdr->value_section);
        }

        store_item->value_offset = offset + store_hdr->value_section.used;
        store_item->value_len = value->len + 1;

        memcpy(p, value->data, value->len);
        p += value->len;
        *p = '\0';

        store_hdr->value_section.used += store_item->value_len;
        return SKV_OK;
    }


    inline int skv_store_item_replace(skv_ctx *ctx,
                                      skv_store_item *store_item,
                                      skv_str *value) {
        char *p = nullptr;
        skv_store_hdr *store_hdr = ctx->store_hdr;
        size_t offset;
        int ret;

        // 使用snapshot重建的store内包含一组重复，重复最新输入的这组key-value
        if ((store_hdr->value_section.used + (value->len + 1)) >
            store_hdr->value_section.size) {
            SPDLOG_INFO("Start to compact the value section.");

            offset = SKV_STORE_GET_SNAPSHOT_OFFSET(store_hdr->value_section);

            ret = skv_store_value_compaction(ctx, value, &p);
            if (ret != SKV_OK) {
                return ret;
            }
        } else {
            p = SKV_STORE_GET_FREE_ADDRESS(store_hdr, store_hdr->value_section);
            offset = SKV_STORE_GET_OFFSET(store_hdr->value_section);
        }

        //  copy the data of the value to the cache
        store_item->value_offset = offset + store_hdr->value_section.used;
        store_item->value_len = value->len + 1;

        //    字符串末尾添加 \0结尾
        memcpy(p, value->data, value->len);
        p += value->len;
        *p = '\0';

        store_hdr->value_section.used += store_item->value_len;

        return SKV_OK;
    }

    // 递归算法实现二分法排序
    inline int skv_item_qsort_for_b_search(void *elt,
                                           void *arg,
                                           void *base,
                                           size_t size,
                                           int low,
                                           int high,
                                           int (*cmp)(const void *ptr_a, const void *ptr_b, void *arg)) {
        int ret, mid;

        if (high <= low) {
            ret = cmp(elt, (char *) base + low * size, arg);
            if (ret > 0) {
                return low + 1;
            }
            return low;
        }

        mid = (low + high) / 2;

        ret = cmp(elt, (char *) base + mid * size, arg);
        if (ret == 0) {
            return mid + 1;
        } else if (ret > 0) {
            return skv_item_qsort_for_b_search(elt, arg, base, size, mid + 1, high, cmp);
        } else {
            return skv_item_qsort_for_b_search(elt, arg, base, size, low, mid - 1, cmp);
        }
    }

    // bsearch二分法索引配套的算法
    inline void skv_item_sort(void *base,
                              size_t n,
                              size_t size,
                              void *arg,
                              int (*cmp)(const void *ptr_a, const void *ptr_b, void *arg)) {
        char *p;
        int pos, count;

        count = (int) (n) -1;

        p = (char *) malloc(size);
        if (p == nullptr) {
            return;
        }

        // take the last element to compare to the previous one
        memcpy(p, (char *) base + count * size, size);

        pos = skv_item_qsort_for_b_search(p, arg, base, size, 0, count, cmp);
        if (pos < count) {
            memmove((char *) base + (pos + 1) * size, (char *) base + pos * size, (count - pos) * size);
            memcpy((char *) base + pos * size, p, size);
        }

        free(p);
    }

    // item 本身进行排序，根据key的值
    inline int store_item_qsort_compare(const void *p1,
                                        const void *p2,
                                        void *arg) {
        int ret;
        char *store_hdr;
        skv_store_item_wrapper *store_item_wrapper;

        ret = (int) ((skv_store_item *) p1)->key_len - (int) ((skv_store_item *) p2)->key_len;
        if (ret != 0) {
            return ret;
        }

        store_item_wrapper = (skv_store_item_wrapper *) arg;
        store_hdr = (char *) store_item_wrapper->ctx->store_hdr;
        if (store_item_wrapper->key_offset == 0) {
            ret = memcmp(store_hdr + ((skv_store_item *) p1)->key_offset,
                         store_hdr + ((skv_store_item *) p2)->key_offset,
                         ((skv_store_item *) p1)->key_len);
        } else {
            ret = memcmp(store_hdr + store_item_wrapper->key_offset,
                         store_hdr + ((skv_store_item *) p2)->key_offset,
                         ((skv_store_item *) p1)->key_len);
        }
        return ret;
    }


    // 注意 set操作里面，双buffer的切换操作每次切换的是item的双buffer， key和value的双buffer是在写满数据之后才进行切换，不跟随set操作切换buffer
    inline int skv_store_set(skv_ctx *ctx,
                             skv_str *key,
                             skv_str *value) {
        int ret;
        skv_store_item *p_cache_item = nullptr;
        skv_store_section *p_item_section;
        skv_store_item_wrapper store_item_wrapper;

        //  generate the snapshot of the cache items
        skv_store_item_make_snapshot(ctx);
        p_item_section = &ctx->store_hdr->item_section;

        store_item_wrapper.ctx = ctx;
        store_item_wrapper.key_offset =
                (int64_t) (key->data - (char *) ctx->store_hdr);
        store_item_wrapper.store_item.key_len = key->len + 1;

        // search snapshot
        p_cache_item = static_cast<skv_store_item *>(bsearch(&store_item_wrapper.store_item,
                                                             ctx->store_items_snapshot,
                                                             p_item_section->used,
                                                             sizeof(skv_store_item),
                                                             skv_store_item_key_compare));

        if (p_cache_item == nullptr) {
            SPDLOG_TRACE("Add key: {} - {}", key->data, value->data);
            ret = skv_store_item_add(ctx, ctx->store_items_snapshot + p_item_section->used, key, value);
        } else {
            SPDLOG_TRACE("Replace key {} with {}", key->data, value->data);
            ret = skv_store_item_replace(ctx, p_cache_item, value);
        }

        if (ret != SKV_OK) {
            return ret;
        }

        // Switch the cache item section if this key doesn't exist
        if (p_cache_item == nullptr) {
            p_item_section->used++;
            store_item_wrapper.key_offset = 0;
            skv_item_sort(ctx->store_items_snapshot,
                          ctx->store_hdr->item_section.used,
                          sizeof(skv_store_item),
                          &store_item_wrapper,
                          store_item_qsort_compare);
        }

        // item的buffer切换 实现lock-free
        p_item_section->id = SKV_STORE_GET_SEC_SNAPSHOT_ID(ctx->store_hdr->item_section);
        return SKV_OK;
    }


    inline int skv_store_del(skv_ctx *ctx,
                             skv_str *key) {
        skv_store_item *p_cache_item = nullptr;
        skv_store_section *p_item_section = nullptr;
        skv_store_item_wrapper store_item_wrapper;

        //  generate the snapshot of the cache items
        skv_store_item_make_snapshot(ctx);

        p_item_section = &ctx->store_hdr->item_section;

        store_item_wrapper.ctx = ctx;
        store_item_wrapper.key_offset =
                (int64_t) (key->data - (char *) ctx->store_hdr);
        store_item_wrapper.store_item.key_len = key->len + 1;

        p_cache_item = static_cast<skv_store_item *>(bsearch(&store_item_wrapper.store_item,
                                                             ctx->store_items_snapshot,
                                                             p_item_section->used,
                                                             sizeof(skv_store_item),
                                                             skv_store_item_key_compare));
        if (p_cache_item == nullptr) {
            SPDLOG_TRACE("Not found key: {}", key->data);
            return SKV_NOT_FOUND;
        }

        if ((p_cache_item - ctx->store_items_snapshot + 1) < p_item_section->used) {
            memcpy(p_cache_item, p_cache_item + 1,
                   sizeof(skv_store_item) * (p_item_section->used -
                                             (p_cache_item - ctx->store_items_snapshot) + 1));
        }

        p_item_section->id = SKV_STORE_GET_SEC_SNAPSHOT_ID(ctx->store_hdr->item_section);
        p_item_section->used--;

        return SKV_OK;
    }


    inline int skv_cache_dump(skv_ctx *ctx) {
        int i;
        skv_store_item *p_item = nullptr;

        SPDLOG_TRACE("------- SKV DUMP -------");
        (void) p_item;
        p_item = ctx->store_items;
        for (i = 0; i < ctx->store_hdr->item_section.used; i++) {
            SPDLOG_TRACE("index: {}, key: {}, value: {}", i,
                         (char *) ctx->store_hdr + p_item[i].key_offset,
                         (char *) ctx->store_hdr + p_item[i].value_offset);
        }

        SPDLOG_TRACE("--------------");

        p_item = ctx->store_items_snapshot;
        for (i = 0; i < ctx->store_hdr->item_section.used; i++) {
            SPDLOG_TRACE("index: {}, key: {}, value: {}", i,
                         (char *) ctx->store_hdr + p_item[i].key_offset,
                         (char *) ctx->store_hdr + p_item[i].value_offset);
        }

        return SKV_OK;
    }

    inline int skv_store_stats(skv_ctx *ctx,
                               skv_str *stat) {
        char buf[1024];
        char *p = buf;

        stat->len = sprintf(
                p,
                "{\n"
                "\t\"item\" : {\n"
                "\t\t\"size\": %u,\n"
                "\t\t\"used\": %u,\n"
                "\t\t\"id\": %d,\n"
                "\t\t\"section-1\": %zu,\n"
                "\t\t\"section-2\": %zu,\n"
                "\t},\n"
                "\t\"key\" : {\n"
                "\t\t\"size\": %u,\n"
                "\t\t\"used\": %u,\n"
                "\t\t\"id\": %d,\n"
                "\t\t\"section-1\": %zu,\n"
                "\t\t\"section-2\": %zu,\n"
                "\t},\n"
                "\t\"value\" : {\n"
                "\t\t\"size\": %u,\n"
                "\t\t\"used\": %u,\n"
                "\t\t\"id\": %d,\n"
                "\t\t\"section-1\": %zu,\n"
                "\t\t\"section-2\": %zu,\n"
                "\t}\n"
                "}\n",
                ctx->store_hdr->item_section.size, ctx->store_hdr->item_section.used,
                ctx->store_hdr->item_section.id,
                ctx->store_hdr->item_section.sec1_offset,
                ctx->store_hdr->item_section.sec2_offset,
                ctx->store_hdr->key_section.size, ctx->store_hdr->key_section.used,
                ctx->store_hdr->key_section.id, ctx->store_hdr->key_section.sec1_offset,
                ctx->store_hdr->key_section.sec2_offset,
                ctx->store_hdr->value_section.size, ctx->store_hdr->value_section.used,
                ctx->store_hdr->value_section.id,
                ctx->store_hdr->value_section.sec1_offset,
                ctx->store_hdr->value_section.sec2_offset);

        stat->data = buf;

        skv_cache_dump(ctx);

        return SKV_OK;
    }

    inline int skv_store_finish(skv_ctx *ctx) {
        int ret;

        ret = skv_shm_sync(ctx);
        if (ret != SKV_OK) {
            return ret;
        }

        ret = skv_shm_finish(ctx);
        if (ret != SKV_OK) {
            return ret;
        }

        return SKV_OK;
    }
}// namespace ots::shmkv

#endif//PYSHMKV_SKV_STORE_H
