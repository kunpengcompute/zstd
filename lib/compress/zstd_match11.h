#include <sys/time.h>
 
#include "zstd_compress_internal.h"
 
/* *********************************
 *  GREEDY Hash Chain
 ***********************************/
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;
typedef int s32;
typedef u32 mf_pos_t;
typedef unsigned char u8;
 
#undef forceinline
#undef unlikely
#undef likely
 
#define forceinline __attribute__((always_inline))
 
#define HC_MATCHFINDER_HASH4_ORDER 15 // TODO: check ARM
#define MATCHFINDER_WINDOW_SIZE    (1UL << 15)
 
#define BASE 4
 
#define HH 0xCF1BBCDCB7A56463ULL
 
#define likely(expr)   __builtin_expect(!!(expr), 1)
#define unlikely(expr) __builtin_expect(!!(expr), 0)
 
struct hc_matchfinder {
    mf_pos_t hash4_tab[(1UL << HC_MATCHFINDER_HASH4_ORDER)];
    mf_pos_t next4_tab[MATCHFINDER_WINDOW_SIZE];
 
    u8 *start;
};
 
#define BITS_H 15
 
inline uint8_t mod15(uint8_t a)  // should return value in range [0, 14]
{
    return a & 7;  // better on ARM
}
 
inline uint8_t small_h(uint8_t c)
{
    return mod15(c);
}
 
#define PRF (8)
static forceinline void GREEDY_HcMatchfinderSkipBytes(struct hc_matchfinder *const restrict mf,
                                                   const u8 *const restrict in_base, const u8 *restrict in_next,
                                                   int count)
{
    uint32_t h4[PRF];
    uint32_t b4[PRF];
 
#define build_chain(i)                                                                                 \
    h4[i] = (MEM_read64(in_next + i) * 0xB7A56463ADA1F189) >> (BASE * 8 - HC_MATCHFINDER_HASH4_ORDER); \
    h4[i] &= (1 << HC_MATCHFINDER_HASH4_ORDER) - 1;                                                    \
    b4[i] = (((uint32_t)(cur_pos + i)) << BITS_H) | (1 << small_h(*(in_next + i + BASE)));             \
    __builtin_prefetch(mf->hash4_tab + h4[i], 1, 3);                                                   \
    mf->next4_tab[cur_pos + i] = mf->hash4_tab[h4[i]];                                                 \
    mf->hash4_tab[h4[i]] = (mf->hash4_tab[h4[i]] & 0x1FFF) | b4[i];

    int cur_pos = 0;
    for (; cur_pos + PRF - 1 < count; cur_pos += PRF) {
        build_chain(0) build_chain(1) build_chain(2) build_chain(3) \
        build_chain(4) build_chain(5) build_chain(6) build_chain(7)
        in_next += PRF;
    }
    for (; cur_pos + BASE < count; cur_pos++) {
        build_chain(0);
        in_next++;
    }
}
 
static __attribute__((always_inline, hot)) void GREEDY_HcNewWindow(struct hc_matchfinder *const restrict mf, const u8 *base, u32 count)
{
    mf->start = base;
    memset(mf->hash4_tab, 0, sizeof(mf->hash4_tab));
    GREEDY_HcMatchfinderSkipBytes(mf, base, base, count);
}
 
static __attribute__((always_inline, hot)) u32 GREEDY_HcMatchfinderLongestMatch(struct hc_matchfinder *const restrict mf,
                                                                             const u8 *const restrict in_base,
                                                                             u8 *const restrict in_next, u32 best_len,
                                                                             size_t *const restrict offset_ret,
                                                                             const u8 *const iLimit)
{
    u32 cur_pos = in_next - in_base;
    u8 *const restrict in_nextcp = in_next;
    const u8 *best_matchptr = 0;
    const u8 *matchptr;
 
    u32 cur_node4 = mf->next4_tab[cur_pos];
    u32 try_next = mf->next4_tab[cur_pos + best_len - (BASE - 1)];
    int minus = 0;
 
    if (unlikely(!(cur_node4 & (1 << small_h(*(in_next + BASE)))))) {
        if (best_len < BASE) {
            #if BASE == 6
            u64 seq = read48(in_next);
            if (likely(read48(in_base + (cur_node4 >>= BITS_H)) == seq)) {
                *offset_ret = cur_pos - cur_node4;
                return BASE;
            }
            #elif BASE == 4
            u64 seq = MEM_read32(in_next);
            if (likely(MEM_read32(in_base + (cur_node4 >>= BITS_H)) == seq)) {
                *offset_ret = STORE_OFFSET(cur_pos - cur_node4);
                return BASE;
            }
            #elif BASE == 5
            u64 seq = read40(in_next);
            if (likely(read40(in_base + (cur_node4 >>= BITS_H)) == seq)) {
                *offset_ret = cur_pos - cur_node4;
                return BASE;
            }
            #endif
        }
        return 0;
    }
 
    if (try_next < cur_node4) {
        cur_node4 = try_next;
        minus = best_len - (BASE - 1);
    }
    int iter = 4;
 
    u32 v = (1 << small_h(*(in_next + minus + BASE)));
    for (; iter && (cur_node4 >>= BITS_H) > minus; iter--) {
        matchptr = in_base + cur_node4 - minus;
        u32 idx = cur_node4;
        cur_node4 = mf->next4_tab[cur_node4];
        if (!(cur_node4 & v)) {
            u32 len = ZSTD_count(in_next, matchptr, iLimit);
            if (len > best_len) {
                best_len = len;
                best_matchptr = matchptr;
            }
            goto out;  // no longer match
        }
        if (MEM_read32(matchptr + best_len - 3) != MEM_read32(in_next + best_len - 3))
            continue;
 
        u32 len = ZSTD_count(in_next, matchptr, iLimit);
        if (len > best_len) {
            best_len = len;
            best_matchptr = matchptr;
            if (cur_node4 > mf->next4_tab[idx - minus + len - BASE]) {
                cur_node4 = mf->next4_tab[idx - minus + len - BASE];
                minus = len - BASE;
                v = (1 << small_h(*(in_next + minus + BASE)));
            }
        } else {
            if (minus != best_len - (BASE - 1) && cur_node4 > mf->next4_tab[idx - minus + best_len - (BASE - 1)]) {
                cur_node4 = mf->next4_tab[idx - minus + best_len - (BASE - 1)];
                minus = best_len - (BASE - 1);
                v = (1 << small_h(*(in_next + minus + BASE)));
            }
        }
    }
out:
    if(best_matchptr == 0) {
        return 0;
    }
    *offset_ret = STORE_OFFSET(in_next - best_matchptr);
    return best_len;
}
 
static __attribute__((always_inline, hot)) size_t ZSTD_GREEDY_HcFindBestMatch(struct hc_matchfinder *const restrict mf, const BYTE *ip,
                                                                           const BYTE *const iLimit, size_t *offsetPtr,
                                                                           u32 bestLen)
{
    return GREEDY_HcMatchfinderLongestMatch(mf, mf->start, ip, bestLen < BASE ? (BASE - 1) : bestLen, offsetPtr, iLimit);
}
 
static __attribute__((always_inline, hot)) size_t ZSTD_GREEDY_HcFindBestMatchLazy(struct hc_matchfinder *const restrict mf, const BYTE *ip,
                                                                               const BYTE *const iLimit,
                                                                               size_t *offsetPtr, u32 bestLen)
{
    return GREEDY_HcMatchfinderLongestMatch(mf, mf->start, ip, bestLen < BASE ? (BASE - 1) : bestLen, offsetPtr, iLimit);
}