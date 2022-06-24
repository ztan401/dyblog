//
// Created by 稻草人 on 2021/12/26.
// 配置总控

#ifndef PYSHMKV_CONFIG_H
#define PYSHMKV_CONFIG_H

// skv，shm kv store
/// 数据库操作模式，readonly/readwrite
#define SKV_OP_TYPE_RO 1//  read-only
#define SKV_OP_TYPE_RW 2//  read-write

/// 数据库状态
#define SKV_OK 0
#define SKV_ERROR -1
#define SKV_OVERFLOW -2
#define SKV_NOT_FOUND -3

///数据库参数设置
#define SKV_KEY_AVERAGE_LEN 64
//#define SKV_VALUE_AVERAGE_LEN 256

/// 双buffer id
#define SKV_STORE_SECTION_ONE 1
#define SKV_STORE_SECTION_TWO 2


/// 双buffer切换函数，当前状态1为store，则状态2为快照，写操作在快照中进行
// 获取双buffer的"相对位置"
#define SKV_STORE_GET_SEC_SNAPSHOT_ID(section)                     \
    ((section.id == SKV_STORE_SECTION_ONE) ? SKV_STORE_SECTION_TWO \
                                           : SKV_STORE_SECTION_ONE)

#define SKV_STORE_GET_OFFSET(section)                            \
    ((section.id == SKV_STORE_SECTION_ONE) ? section.sec1_offset \
                                           : section.sec2_offset)

#define SKV_STORE_GET_SNAPSHOT_OFFSET(section)                   \
    ((section.id == SKV_STORE_SECTION_ONE) ? section.sec2_offset \
                                           : section.sec1_offset)

// 获取双buffer内存的首位置，"直接可操作的内存地址"
#define SKV_STORE_GET_ADDRESS(store_hdr, section)       \
    ((section.id == SKV_STORE_SECTION_ONE)              \
             ? (char *) store_hdr + section.sec1_offset \
             : (char *) store_hdr + section.sec2_offset)

#define SKV_STORE_GET_SNAPSHOT_ADDRESS(store_hdr, section) \
    ((section.id == SKV_STORE_SECTION_ONE)                 \
             ? (char *) store_hdr + section.sec2_offset    \
             : (char *) store_hdr + section.sec1_offset)

#define SKV_STORE_GET_FREE_ADDRESS(store_hdr, section)                 \
    ((section.id == SKV_STORE_SECTION_ONE)                             \
             ? (char *) store_hdr + section.sec1_offset + section.used \
             : (char *) store_hdr + section.sec2_offset + section.used)


// 自定义数据结构的写入操作， str数据类型， value是值
//#define skv_string_set(str, value)  \
//    (str)->len = sizeof(value) - 1; \
//    (str)->data = (char *) value

#endif//PYSHMKV_CONFIG_H
