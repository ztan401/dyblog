//
// Created by 稻草人 on 2021/12/28.
//

#ifndef SHMKV_SKV_DATA_H
#define SHMKV_SKV_DATA_H


namespace ots::shmkv {
#include <cstdio>

    /// 自定义string
    // C语言中自定义String，为SKV数据库自定义的String数据类型
    typedef struct {
        unsigned int len;
        char *data;
    } skv_str;


    // item 数据存储，是数据库的索引
    typedef struct {
        size_t key_offset;
        size_t value_offset;
        unsigned int key_len;
        unsigned int value_len;
    } skv_store_item;


    // sec1和sec2分别表示双buffer的内存首位置
    // size表示预设大小，used表示实际使用量, char长度
    // id=1/2 表示当前使用的buffer是哪个
    typedef struct {
        unsigned int id;
        unsigned int size;
        unsigned int used;
        size_t sec1_offset;
        size_t sec2_offset;
    } skv_store_section;


    // 内存结构图里面的header， 存储item、key、value三部分双buffer的位置信息
    typedef struct {
        skv_store_section item_section;
        skv_store_section key_section;
        skv_store_section value_section;
    } skv_store_hdr;


    // 共享内存参数
    typedef struct {
        int fd;
        char *path;
        void *data;
        unsigned long size;
        char exists;
    } skv_shm_store;


    // data swap/switch pointer
    // header 内存首位置
    // item 内存首位置
    // item 快照内存首位置
    typedef struct {
        // initialization switch
        int init;
        skv_shm_store shm;
        skv_store_hdr *store_hdr;
        skv_store_item *store_items;
        // All write operations first modify the snapshot area.
        skv_store_item *store_items_snapshot;
    } skv_ctx;


    //  这里的设计逻辑是在共享内存外存在一个数据，
    //  使用数据结构skv_store_item表示，
    //  并且添加这个数据结构相对于共享内存的偏移量
    typedef struct {
        skv_store_item store_item;
        skv_ctx *ctx;
        size_t key_offset;
    } skv_store_item_wrapper;
}// namespace ots::shmkv


#endif//SHMKV_SKV_DATA_H
