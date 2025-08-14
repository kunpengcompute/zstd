/*
* 版权所有 (c) 华为技术有限公司 2025
*/
#ifndef MEM_H
#define MEM_H

#include <stdint.h>
#include <arm_neon.h>
#include "decompress/zstd_decompress_block.h"

static inline void MEM_COPY16B (uint8_t *p_dst, const uint8_t *p_src) {
    vst1q_u8(p_dst, vld1q_u8(p_src));
}

static inline void MEM_COPY (uint8_t *p_dst, const uint8_t *p_src, int len) {
    do {
        vst1q_u8(p_dst, vld1q_u8(p_src));
        p_dst += 16;
        p_src += 16;
        len   -= 16;
    } while (len > 0);
}

static inline void MEM_SET_1B (uint8_t *p_dst, const uint8_t value, int len) {
    uint8x16_t vec_data = vdupq_n_u8(value);
    do {
        vst1q_u8(p_dst, vec_data);
        p_dst += 16;
        len   -= 16;
    } while (len > 0);
}

static inline void MEM_SET_2B (uint8_t *p_dst, const uint16_t value, int len) {
    uint16x8_t vec_data = vdupq_n_u16(value);
    do {
        vst1q_u16((uint16_t*)p_dst, vec_data);
        p_dst += 16;
        len   -= 16;
    } while (len > 0);
}

static inline void MEM_SET_4B (uint8_t *p_dst, const uint32_t value, int len) {
    uint32x4_t vec_data = vdupq_n_u32(value);
    do {
        vst1q_u32((uint32_t*)p_dst, vec_data);
        p_dst += 16;
        len   -= 16;
    } while (len > 0);
}

static inline void MEM_LZ_MOVE (uint8_t *p_dst, uint8_t *p_match, int32_t ml, int32_t of) {
    uint8x16_t vec_data = vld1q_u8(p_match);
    do {
        vst1q_u8(p_dst, vec_data);
        p_dst += of;
        ml    -= of;
    } while (ml > 0);
}

static inline int8_t trailbit_u64 (uint64_t val) {
    return (int8_t)__builtin_ctzll(val);
}

static inline int8_t highbit_u9 (uint16_t x) {
    return 31 - __builtin_clz((uint32_t)x);
}

static void ZSTD_copy4(void* dst, const void* src) { ZSTD_memcpy(dst, src, 4); }

static void ZSTD_overlapCopy8(BYTE** op, BYTE const** ip, size_t offset) {
    assert(*ip <= *op);
    if (offset < 8) {
        /* close range match, overlap */
        static const U32 dec32table[] = { 0, 1, 2, 1, 4, 4, 4, 4 };
        static const int dec64table[] = { 8, 8, 8, 7, 8, 9, 10, 11};
        int const sub2 = dec64table[offset];
        (*op)[0] = (*ip)[0];
        (*op)[1] = (*ip)[1];
        (*op)[2] = (*ip)[2];
        (*op)[3] = (*ip)[3];
        *ip += dec32table[offset];
        ZSTD_copy4(*op+4, *ip);
        *ip -= sub2;
    } else {
        ZSTD_copy8(*op, *ip);
    }
    *ip += 8;
    *op += 8;
    assert(*op - *ip >= 8);
}

#endif // MEM_H