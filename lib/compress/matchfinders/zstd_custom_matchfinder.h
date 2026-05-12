#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>
#include "../zstd_compress_internal.h"
#include "stats.h"
/* *********************************
 *  WRC Hash Chain Matchfinder
 ***********************************/
#define MAX_BLOCK_SIZE             (128 * 1024) // internal ZSTD block size
#define HC_MATCHFINDER_HASH4_ORDER 20
#define HC_MATCHFINDER_HASH4_SMALL_ORDER 15
#define MATCHFINDER_WINDOW_SIZE    ((1UL << 20) * 3)
#define LEAVE_AFTER_SHIFT          (1 << 19)
#define HC_NEXT4_TAB_SIZE          (512 * 1024)
#define HC_NEXT4_TAB_MASK          (HC_NEXT4_TAB_SIZE - 1)
#define CACHE_FRIENDLY_DIST        ((512 - 128) * 1024)
#define BITS_H                     8       // Shift used to separate 24-bit pos from 8-bit bloom filter
#define PRF                        1024    // Chunk size (large to amortize pipeline fill/drain)
#define PIPE                       32      // Per-stream pipeline depth (controls MLP), power of 2
#define HIGH_CR_HASH_DISCOUNT      2
#define PRESCAN_STEP               32
#define PRESCAN_HLOG               13
#define PRESCAN_HSIZE              (1u << PRESCAN_HLOG)
#define LONG_MATCH_BORDER          10
#define LONG_MATCH_LEN             22
#define small_h(a)                 (a & 7)
#define PRF_SMALL                  8
#define BASE                       4

 /*
 * Note: mf_pos_t stores data packed as: [24 bits for Position | 8 bits for Bloom filter]
 * Bloom Filter: stores whether the base_val-th byte value occurs anywhere on a previous
 *               hash chain entry.
 */
struct hc_matchfinder {
    mf_pos_t next4_tab[HC_NEXT4_TAB_SIZE];               /* 2MB */
    mf_pos_t hash4_tab[1 << HC_MATCHFINDER_HASH4_ORDER]; /* 4MB */
    mf_pos_t hash4_small_tab[1 << HC_MATCHFINDER_HASH4_SMALL_ORDER];    // 128KB: Hash table heads
    uint8_t  ext_dict_buffer[MATCHFINDER_WINDOW_SIZE];   /* 3MB */
    uint32_t positions[MAX_BLOCK_SIZE];                  /* 512KB */
    uint32_t scan_tab[PRESCAN_HSIZE];                    /* 32KB */
 
    uint32_t stream_start_curr;   /* Absolute ZSTD index that maps to WRC pos 0. */
    uint32_t last_curr_end;       /* Expected `curr` for the next block, for continuity check */
    uint32_t base_val;            /* Min match length: 5/6/7/8 */
    uint8_t initialized;   // bool
    uint8_t ext_dict_used; // bool
    uint8_t high_cr;      // bool
};
#include "memory_helpers.h"

static inline __attribute__((always_inline, hot)) void WRC_small_block_HcUpdateWindow(
    struct hc_matchfinder *const restrict mf,
    const u8 *const restrict in_base,
    const u8 *restrict in_next,
    u32 srcSize,
    const int base_val)
{
    uint32_t h4[PRF_SMALL];
    uint32_t b4[PRF_SMALL];

    #define build_chain(i)                                                                                      \
        h4[i] = (MEM_read64(in_next + i) * 889523592379ULL) >> (BASE * 8 - HC_MATCHFINDER_HASH4_SMALL_ORDER);   \
        h4[i] &= (1 << HC_MATCHFINDER_HASH4_SMALL_ORDER) - 1;                                                   \
        b4[i] = (uint32_t)(pos + i);                                                                            \
        __builtin_prefetch(mf->hash4_small_tab + h4[i], 1, 3);                                                  \
        mf->next4_tab[pos + i] = mf->hash4_small_tab[h4[i]];                                                    \
        mf->hash4_small_tab[h4[i]] = b4[i];

    int pos = 0;
    for (; pos + PRF_SMALL - 1 < srcSize; pos += PRF_SMALL) {
        build_chain(0) build_chain(1) build_chain(2) build_chain(3) \
        build_chain(4) build_chain(5) build_chain(6) build_chain(7)
        in_next += PRF_SMALL;
    }

    for (; pos + BASE < srcSize; pos++) {
        build_chain(0);
        in_next++;
    }
}

static inline __attribute__((always_inline, hot)) u32 WRC_small_block_HcMatchfinderLongestMatch(
    struct hc_matchfinder *const restrict mf,
    const u8 *const restrict in_base, u8 *const restrict in_next,
    u32 best_len, size_t *const restrict offset_ret,
    const u8 *const iLimit, u32 our_windowLow, const int base_val)
{
    u32 cur_pos = in_next - in_base;
    // u32 cur_pos = 0;
    u8 *const restrict in_nextcp = in_next;
    const u8 *best_matchptr = 0;
    const u8 *matchptr;
 
    u32 cur_node4 = mf->next4_tab[cur_pos];
    u32 try_next = mf->next4_tab[cur_pos + best_len - (BASE - 1)];
    int minus = 0;
 
    if (try_next < cur_node4) {
        cur_node4 = try_next;
        minus = best_len - (BASE - 1);
    }
    int iter = 4;

    u32 v = (1 << small_h(*(in_next + minus + BASE)));
    best_len = 3;
    for (; iter && cur_node4 > minus; iter--) {
        matchptr = in_base + cur_node4 - minus;
        u32 idx = cur_node4;

        if (MEM_read32(matchptr + best_len - 3) != MEM_read32(in_next + best_len - 3))
            continue;
 
        u32 len = ZSTD_count(in_next, matchptr, iLimit);
        if (len > best_len) {
            best_len = len;
            best_matchptr = matchptr;
        }
        cur_node4 = mf->next4_tab[cur_node4];
    }

out:
    if(best_matchptr == 0) {
        return 0;
    }
    *offset_ret = STORE_OFFSET(in_next - best_matchptr);
    return best_len;
}

static inline __attribute__((always_inline, hot)) size_t ZSTD_small_block_WRC_HcFindBestMatch(
    struct hc_matchfinder *const restrict mf, const BYTE *our_base, const BYTE *ip,
    const BYTE *const iLimit, size_t *offsetPtr, u32 bestLen, u32 our_windowLow, const int base_val)
{
    size_t ret = WRC_small_block_HcMatchfinderLongestMatch(mf, our_base, (u8*)ip, 
                     BASE - 1, offsetPtr, iLimit, our_windowLow, base_val);
    return ret;
}

static inline __attribute__((always_inline, hot)) size_t ZSTD_small_block_compressBlock_WRC_internal(
    ZSTD_MatchState_t *ms, SeqStore_t *seqStore,
    U32 rep[ZSTD_REP_NUM], const void *src, size_t srcSize, const int base_val,
    const BYTE* our_base, const BYTE* match_src)
{
    if((BYTE*)src  - (ms->window.base + ms->window.dictLimit) != 0 /*|| dictMode != 0 */) {
        DEBUGLOG(5, "WARNING: We don't use previous block nor dict");
    }
 
    const BYTE *const base = ms->window.base;
    U32 const curr = (U32)((const BYTE *)src - base);
    U32 const windowLow = ZSTD_getLowestPrefixIndex(ms, curr, ms->cParams.windowLog);
    struct hc_matchfinder *WRC_mf = (struct hc_matchfinder *)ms->WRC_matchfinder;

    U32 maxRep = curr - windowLow; 
    U32 const our_pos = curr - WRC_mf->stream_start_curr;
    U32 const our_windowLow = (windowLow > WRC_mf->stream_start_curr) ? (windowLow - WRC_mf->stream_start_curr) : 0;
 
    WRC_small_block_HcUpdateWindow(WRC_mf, our_base, match_src, srcSize, base_val);

    const BYTE *const istart = (const BYTE *)src;
    const BYTE *ip = istart;
    const BYTE *anchor = istart;
    const BYTE *const iend = istart + srcSize;
    const BYTE *const ilimit = iend - 8;
 
    U32 offset_1 = rep[0], offset_2 = rep[1];
    U32 offsetSaved1 = 0, offsetSaved2 = 0;
 
    ip += 1; // dict or previous block not supported
 
    if (offset_2 > maxRep)
        offsetSaved1 = offset_2, offset_2 = 0;
    if (offset_1 > maxRep)
        offsetSaved2 = offset_1, offset_1 = 0;
 
    offset_1 = offset_2 = 0;
 
    while (ip < ilimit) {
        size_t matchLength = 1;
        size_t offcode = REPCODE1_TO_OFFBASE;
        const BYTE *start = ip + 1;
 
        if (((offset_1 > 0) & (MEM_read32(ip + 1 - offset_1) == MEM_read32(ip + 1)))) {
            matchLength = ZSTD_count(ip + 1 + 4, ip + 1 + 4 - offset_1, iend) + 4;
            if (matchLength > 10)
                goto use;
        }
 
        {
            size_t offsetFound = 999999999;
            size_t const ml2 = ZSTD_small_block_WRC_HcFindBestMatch(WRC_mf, our_base, ip, iend, &offsetFound, (int)matchLength, our_windowLow, base_val);
            if (ml2 > matchLength)
                matchLength = ml2, start = ip, offcode = offsetFound;
        }
 
        if (matchLength < 4) {
            int skip = 1;
            ip += skip;
            continue;
        }
 
        while (ip < ilimit) {
            ip++;
            size_t offsetFound = 999999999;
 
            if ((offcode) && ((offset_1 > 0) & (MEM_read32(ip) == MEM_read32(ip - offset_1)))) {
                int const mlRep = ZSTD_count(ip + 4, ip + 4 - offset_1, iend) + 4;
                int const gain2 = (int)(mlRep * 3);
                int const gain1 = (int)(matchLength * 3 - ZSTD_highbit32((U32)STORED_TO_OFFBASE(offcode)) + 1);
                if ((mlRep >= 4) && (gain2 > gain1)) {
                    matchLength = mlRep, offcode = REPCODE1_TO_OFFBASE, start = ip;
                }
            }
            {
                int const ml2 = ZSTD_small_block_WRC_HcFindBestMatch(WRC_mf, our_base, ip, iend, &offsetFound, (int)matchLength, our_windowLow, base_val);
 
                int const gain2 =
                    (int)(ml2 * 5 - ZSTD_highbit32((U32)STORED_TO_OFFBASE(offsetFound))); /* raw approx */
                int const gain1 = (int)(matchLength * 5 - ZSTD_highbit32((U32)STORED_TO_OFFBASE(offcode)) + 1);
                if ((ml2 >= 4) && (gain2 > gain1)) {
                    matchLength = ml2, offcode = offsetFound, start = ip;
                    continue; /* search a better one */
                }
            }
            break; /* nothing found : store previous solution */
        }

        while (ip < ilimit) {
            ip++;
            size_t offset2 = 999999999;
 
            if ((offcode) && ((offset_1 > 0) & (MEM_read32(ip) == MEM_read32(ip - offset_1)))) {
                int const mlRep = ZSTD_count(ip + 4, ip + 4 - offset_1, iend) + 4;
                int const gain2 = (int)(mlRep * 3);
                int const gain1 = (int)(matchLength * 3 - ZSTD_highbit32((U32)STORED_TO_OFFBASE(offcode)) + 1);
                if ((mlRep >= 4) && (gain2 > gain1)) {
                    matchLength = mlRep, offcode = REPCODE1_TO_OFFBASE, start = ip;
                }
            }
            {
                int const ml2 = ZSTD_small_block_WRC_HcFindBestMatch(WRC_mf, our_base, ip, iend, &offset2, (int)matchLength, our_windowLow, base_val);
 
                int const gain2 =
                    (int)(ml2 * 5 - ZSTD_highbit32((U32)STORED_TO_OFFBASE(offset2))); /* raw approx */
                int const gain1 = (int)(matchLength * 5 - ZSTD_highbit32((U32)STORED_TO_OFFBASE(offcode)) + 1);
                if ((ml2 >= 4) && (gain2 > gain1)) {
                    matchLength = ml2, offcode = offset2, start = ip;
                    continue; /* search a better one */
                }
            }
 
            break; /* nothing found : store previous solution */
        }
    use:
        /* catch up */
        if (STORED_IS_OFFSET(offcode)) {
            while (((start > anchor) & (start - STORED_OFFSET(offcode) > istart)) &&
                   (start[-1] == (start - STORED_OFFSET(offcode))[-1])) /* only search for offset within prefix */
            {
                start--;
                matchLength++;
            }
            offset_2 = offset_1;
            offset_1 = (U32)STORED_OFFSET(offcode);
        }
 
        /* store sequence */
        size_t const litLength = (size_t)(start - anchor);
        ZSTD_storeSeq(seqStore, litLength, anchor, iend, (U32)offcode, matchLength);
 
        anchor = ip = start + matchLength;
        while (((ip <= ilimit) & (offset_2 > 0)) && (MEM_read32(ip) == MEM_read32(ip - offset_2))) {
            /* store sequence */
            matchLength = ZSTD_count(ip + 4, ip + 4 - offset_2, iend) + 4;
            offcode = offset_2;
            offset_2 = offset_1;
            offset_1 = (U32)offcode; /* swap repcodes */
            ZSTD_storeSeq(seqStore, 0, anchor, iend, REPCODE1_TO_OFFBASE, matchLength);
            ip += matchLength;
            anchor = ip;
            continue; /* faster when present ... (?) */
        }
    }
 
    /* If offset_1 started invalid (offsetSaved1 != 0) and became valid (offset_1 != 0),
     * rotate saved offsets. See comment in ZSTD_compressBlock_fast_noDict for more context. */
    offsetSaved2 = ((offsetSaved1 != 0) && (offset_1 != 0)) ? offsetSaved1 : offsetSaved2;
 
    /* save reps for next block */
    rep[0] = offset_1 ? offset_1 : offsetSaved1;
    rep[1] = offset_2 ? offset_2 : offsetSaved2;
    /* Return the last literals size */
    return (size_t)(iend - anchor);
}

MEM_STATIC size_t ZSTD_countBack(const BYTE* pIn, const BYTE* pMatch, const BYTE* const pMin)
{
    const BYTE* const pStart = pIn;
    while (pIn >= pMin + sizeof(size_t)) {
        size_t const diff = MEM_readST(pIn - sizeof(size_t)) ^ MEM_readST(pMatch - sizeof(size_t));
        if (!diff) { pIn -= sizeof(size_t); pMatch -= sizeof(size_t); continue; }
        pIn -= (sizeof(size_t) == 8 ? __builtin_clzll((uint64_t)diff) : __builtin_clz((uint32_t)diff)) >> 3;
        return (size_t)(pStart - pIn);
    }
    while (pIn > pMin && pIn[-1] == pMatch[-1]) { pIn--; pMatch--; }
    return (size_t)(pStart - pIn);
}

/*
 * Quick linear scan over the block. Adds all valid byte positions to the
 * "positions" array except for the middle bytes of matches >= LONG_MATCH_LEN bytes.
 * First and last LONG_MATCH_BORDER bytes of the match are preserved.
 */
static inline uint32_t WRC_PreScan(
    const uint8_t *restrict src,
    uint32_t                srcSize,
    uint32_t  *restrict     positions,
    uint32_t  *restrict     scan_tab)
{
    if (srcSize < 10000) return srcSize;
    memset(scan_tab, 0, PRESCAN_HSIZE * sizeof(uint32_t));
    
    /* Adaptive step: PRESCAN_STEP on hit, +1 every PRESCAN_STEP bytes of
     * misses, capped at MAX_STEP. Cheap incompressibility acceleration. */
    uint32_t       step      = PRESCAN_STEP;
    uint32_t       next_ramp = 2 * PRESCAN_STEP;
    const uint32_t MAX_STEP  = PRESCAN_STEP * 8;

    uint32_t i              = PRESCAN_STEP;
    uint32_t last_match_end = 0;
    uint32_t total_match    = 0;
    uint32_t pos_idx        = 0;
    uint32_t last_added_pos = 0;
    
    const uint32_t limit = srcSize - 8;

    while (i <= limit) {
        if (i > 10000u && total_match * 3 < i) return srcSize;
        if (i > 30000u && total_match * 2 < i) return srcSize;

        uint64_t val  = MEM_read64(src + i);
        uint32_t h    = (uint32_t)((val * 0x9E3779B97F4A7C15ULL) >> (64u - PRESCAN_HLOG));
        uint32_t prev = scan_tab[h];
        scan_tab[h]   = i;

        if (prev == 0 || MEM_read64(src + prev) != val) {
            if (i >= next_ramp) {
                if (step < MAX_STEP) step++;
                next_ramp += PRESCAN_STEP;
            }
            i += step;
            continue;
        }

        /* Hit: extend in both directions using optimized word-scans */
        step      = PRESCAN_STEP;
        next_ramp = i + PRESCAN_STEP;

        /* Forward extension */
        uint32_t right = 8 + (uint32_t)ZSTD_count(src + i + 8, src + prev + 8, src + srcSize);
        
        /* Backward extension */
        uint32_t max_left = (i - last_match_end < prev) ? i - last_match_end : prev;
        uint32_t left     = (uint32_t)ZSTD_countBack(src + i, src + prev, src + i - max_left);

        uint32_t total_len = left + right;

        if (total_len < LONG_MATCH_LEN) {
            i += (total_len > PRESCAN_STEP) ? total_len : PRESCAN_STEP;
            continue;
        }

        uint32_t match_start = i - left;
        uint32_t match_end   = i + right;

        uint32_t end_add = match_start + LONG_MATCH_BORDER;
        if (end_add > match_end) end_add = match_end;
        if (end_add > limit + 1) end_add = limit + 1;

        for (uint32_t p = last_added_pos; p < end_add; p++) {
            positions[pos_idx++] = p;
        }

        uint32_t next_start = (match_end > LONG_MATCH_BORDER) ? match_end - LONG_MATCH_BORDER : match_end;
        if (next_start < end_add) next_start = end_add;

        last_added_pos  = next_start;
        total_match    += total_len;
        last_match_end  = match_end;
        i               = match_end;
    }

    for (uint32_t p = last_added_pos; p <= limit; p++) {
        positions[pos_idx++] = p;
    }
    
    return pos_idx;
}

/*
 * High MLP Universal Update Method. 
 * 'use_positions' enables branch-compiled array reads natively avoiding stalls. 
 */
static inline __attribute__((always_inline, hot)) void WRC_HcUpdateWindow_internal(
    struct hc_matchfinder *const restrict mf,
    const u8 *const restrict in_base,
    const u8 *restrict in_next,
    u32 srcSize,
    const int base_val,
    const bool use_positions,
    const u32 num_positions)
{
    const uint32_t hash_order  = HC_MATCHFINDER_HASH4_ORDER - mf->high_cr * HIGH_CR_HASH_DISCOUNT;
    const uint32_t hash_shift  = 64 - hash_order;
    const uint32_t input_shift = 64 - (base_val * 8); // Shift bytes to the top of the u64
    const uint64_t prime       = 0xB7A56463ADA1F189ULL;
    const uint32_t base_off    = (uint32_t)base_val - 1;

    u32 cur_pos = (u32)(in_next - in_base);
    const u32 end_pos = cur_pos + srcSize;
    const u32 limit = srcSize - 8;

    if (use_positions) {
        u32 start_idx = cur_pos & HC_NEXT4_TAB_MASK;
        if (start_idx + srcSize <= HC_NEXT4_TAB_SIZE) {
            memset(mf->next4_tab + start_idx, 0, srcSize * sizeof(mf_pos_t));
        } else {
            u32 first_part = HC_NEXT4_TAB_SIZE - start_idx;
            memset(mf->next4_tab + start_idx, 0, first_part * sizeof(mf_pos_t));
            memset(mf->next4_tab, 0, (srcSize - first_part) * sizeof(mf_pos_t));
        }
    }

    uint32_t hbuf0[PIPE];
    uint32_t hbuf1[PIPE];
    
    uint32_t idx = 0;
    const uint32_t batch_size = PIPE * 2;
    const uint32_t pipe_limit = (num_positions >= batch_size) ? num_positions - batch_size : 0;

    if (num_positions >= batch_size) {
        
        /* ── 1. PIPE FILL ──────── */
        #pragma unroll(8)
        for (int i = 0; i < PIPE; i++) {
            uint32_t p0 = use_positions ? mf->positions[idx + 2*i] : (idx + 2*i);
            uint32_t p1 = use_positions ? mf->positions[idx + 2*i + 1] : (idx + 2*i + 1);
            
            hbuf0[i] = (uint32_t)((MEM_read64(in_next + p0) << input_shift) * prime >> hash_shift);
            hbuf1[i] = (uint32_t)((MEM_read64(in_next + p1) << input_shift) * prime >> hash_shift);
            
            __builtin_prefetch(mf->hash4_tab + hbuf0[i], 0, 3);
            __builtin_prefetch(mf->hash4_tab + hbuf1[i], 0, 3);
        }
        idx += batch_size;

        /* ── 2. STEADY STATE ───── */
        for (; idx <= pipe_limit; idx += batch_size) {
            #pragma unroll(8)
            for (int k = 0; k < PIPE; k++) {
                uint32_t proc_idx = idx - batch_size + 2*k;
                uint32_t p0 = use_positions ? mf->positions[proc_idx] : proc_idx;
                uint32_t p1 = use_positions ? mf->positions[proc_idx + 1] : proc_idx + 1;

                uint32_t cur_p0 = cur_pos + p0;
                uint32_t cur_p1 = cur_pos + p1;
                const uint8_t *p_addr0 = in_next + p0;
                const uint8_t *p_addr1 = in_next + p1;

                uint32_t old_h0 = mf->hash4_tab[hbuf0[k]];
                uint32_t old_h1 = mf->hash4_tab[hbuf1[k]];

                mf->next4_tab[cur_p0 & HC_NEXT4_TAB_MASK] = old_h0;
                mf->next4_tab[cur_p1 & HC_NEXT4_TAB_MASK] = old_h1;

                uint32_t new_h0 = old_h0 | (1u << (p_addr0[base_off] & 7));
                __asm__("bfi %w0, %w1, #8, #24" : "+r"(new_h0) : "r"(cur_p0));
                mf->hash4_tab[hbuf0[k]] = new_h0;

                uint32_t new_h1 = old_h1 | (1u << (p_addr1[base_off] & 7));
                __asm__("bfi %w0, %w1, #8, #24" : "+r"(new_h1) : "r"(cur_p1));
                mf->hash4_tab[hbuf1[k]] = new_h1;

                uint32_t next_p0 = use_positions ? mf->positions[idx + 2*k] : (idx + 2*k);
                uint32_t next_p1 = use_positions ? mf->positions[idx + 2*k + 1] : (idx + 2*k + 1);
                
                hbuf0[k] = (uint32_t)((MEM_read64(in_next + next_p0) << input_shift) * prime >> hash_shift);
                hbuf1[k] = (uint32_t)((MEM_read64(in_next + next_p1) << input_shift) * prime >> hash_shift);
                
                __builtin_prefetch(mf->hash4_tab + hbuf0[k], 0, 3);
                __builtin_prefetch(mf->hash4_tab + hbuf1[k], 0, 3);
            }
        }

        /* ── 3. PIPE DRAIN ─────── */
        #pragma unroll(8)
        for (int k = 0; k < PIPE; k++) {
            uint32_t proc_idx = idx - batch_size + 2*k;
            uint32_t p0 = use_positions ? mf->positions[proc_idx] : proc_idx;
            uint32_t p1 = use_positions ? mf->positions[proc_idx + 1] : proc_idx + 1;

            uint32_t cur_p0 = cur_pos + p0;
            uint32_t cur_p1 = cur_pos + p1;
            const uint8_t *p_addr0 = in_next + p0;
            const uint8_t *p_addr1 = in_next + p1;

            uint32_t old_h0 = mf->hash4_tab[hbuf0[k]];
            uint32_t old_h1 = mf->hash4_tab[hbuf1[k]];

            mf->next4_tab[cur_p0 & HC_NEXT4_TAB_MASK] = old_h0;
            mf->next4_tab[cur_p1 & HC_NEXT4_TAB_MASK] = old_h1;

            uint32_t new_h0 = old_h0 | (1u << (p_addr0[base_off] & 7));
            __asm__("bfi %w0, %w1, #8, #24" : "+r"(new_h0) : "r"(cur_p0));
            mf->hash4_tab[hbuf0[k]] = new_h0;

            uint32_t new_h1 = old_h1 | (1u << (p_addr1[base_off] & 7));
            __asm__("bfi %w0, %w1, #8, #24" : "+r"(new_h1) : "r"(cur_p1));
            mf->hash4_tab[hbuf1[k]] = new_h1;
        }
    }

    /* ── 4. TAIL: scalar fallback ── */
    for (; idx < num_positions; idx++) {
        uint32_t p = use_positions ? mf->positions[idx] : idx;
        uint32_t cur_p = cur_pos + p;
        const uint8_t *p_addr = in_next + p;

        uint32_t h = (uint32_t)((MEM_read64(p_addr) << input_shift) * prime >> hash_shift);
        uint32_t old_h = mf->hash4_tab[h];
        mf->next4_tab[cur_p & HC_NEXT4_TAB_MASK] = old_h;

        uint32_t new_h = old_h | (1u << (p_addr[base_off] & 7));
        __asm__("bfi %w0, %w1, #8, #24" : "+r"(new_h) : "r"(cur_p));
        mf->hash4_tab[h] = new_h;
    }

    /* Zero dangling chain entries */
    uint32_t tail_start = cur_pos + limit + 1;
    for (u32 i = tail_start; i < end_pos; i++) {
        mf->next4_tab[i & HC_NEXT4_TAB_MASK] = 0;
    }
}

static inline __attribute__((always_inline, hot)) void WRC_HcUpdateWindow(
    struct hc_matchfinder *const restrict mf,
    const u8 *const restrict in_base,
    const u8 *restrict in_next,
    u32 srcSize,
    const int base_val)
{
    if(srcSize <= 8) {
        u32 cur_pos = (u32)(in_next - in_base);
        for (u32 i = 0; i < srcSize; i++) {
            mf->next4_tab[(cur_pos + i) & HC_NEXT4_TAB_MASK] = 0;
        }
        return;
    }
    u32 limit = srcSize - 8;
    uint32_t num_positions = 0;
    bool use_array = false;
    
    if (srcSize > MAX_BLOCK_SIZE / 2) { // If block is big enough
        uint32_t scanned_pos = WRC_PreScan(in_next, srcSize, mf->positions, mf->scan_tab);
        /* If we have to evaluate < 70% of original bytes, employ the array skip optimization */
        if (scanned_pos < (srcSize * 7) / 10) {
            use_array = true;
            num_positions = scanned_pos;
        } else {
            use_array = false;
            num_positions = limit + 1;
        }
    } else {
        use_array = false;
        num_positions = limit + 1;
    }

    if (use_array) { // compile time branch to avoid stalls from array reads when not used
        WRC_HcUpdateWindow_internal(mf, in_base, in_next, srcSize, base_val, true, num_positions);
    } else {
        WRC_HcUpdateWindow_internal(mf, in_base, in_next, srcSize, base_val, false, num_positions);
    }
}

static inline __attribute__((always_inline, hot)) u32 WRC_HcMatchfinderLongestMatch(
    struct hc_matchfinder *const restrict mf,
    const u8 *const restrict in_base, u8 *const restrict in_next,
    u32 best_len, size_t *const restrict offset_ret,
    const u8 *const iLimit, u32 our_windowLow, const int base_val)
{
    u32 cur_pos = (u32)(in_next - in_base);
    const u8 *best_matchptr = 0;

    u32 cur_node4 = mf->next4_tab[cur_pos & HC_NEXT4_TAB_MASK];
    u32 minus = 0; // Tells which hashchain we are on
    u32 bloom_filter_bit = 1 << (in_next[base_val - 1] & 7); 

    if (!(cur_node4 & bloom_filter_bit)) { 
        // Quickly reject using packed bloom filter if we see no possibility of finding a match
        return 0;
    }

    // Attempt to leap forward using existing chain
    if(unlikely(base_val != best_len + 1)) { // rare: only if repcode match was found, or if lazy used
        u32 try_next = mf->next4_tab[(cur_pos + best_len - (base_val - 1)) & HC_NEXT4_TAB_MASK];
        if (try_next < cur_node4) {
            cur_node4 = try_next;
            minus = best_len - (base_val - 1);
            bloom_filter_bit = 1 << (in_next[best_len] & 7);
        }
    }
        
    const int MAX_ITERS = cur_pos > 2 * 1024 * 1024 ? 3 : 4;
    int iter = MAX_ITERS;

    // If matched - then we can probably extend the match (last 4 positions are the same)
    u32 i32_end = MEM_read32(in_next + best_len - 3);

    // Main match evaluation loop
    for (; iter && (cur_node4 >>= BITS_H) > minus; iter--) {
        u32 match_pos = cur_node4 - minus;
        const u8 *__restrict matchptr = in_base + match_pos;
        
        // Hoist load to hide potential latency of tracking mf->next4_tab chain miss
        u32 next_node4 = mf->next4_tab[cur_node4 & HC_NEXT4_TAB_MASK];
        
        u32 offset = cur_pos - match_pos; // 32-bit unsigned wrap acts as natural forward/garbage guard

        u32 m32_end = MEM_read32(matchptr + best_len - 3);
        u32 diff = (m32_end ^ i32_end); // Branchless

        if (iter == 1 || offset >= CACHE_FRIENDLY_DIST) { // last iteration, no need to read next4_tab, just check a match
            if (!diff) {
                u32 len = ZSTD_count(in_next, matchptr, iLimit);
                if (len > best_len) {
                    best_len = len;
                    best_matchptr = matchptr;
                }
            }
            break; 
        }

        cur_node4 = next_node4;
        
        // Bloom filter check for early exit
        if (!(cur_node4 & bloom_filter_bit)) {
            // We know there is no way to find a match in next iterations, this is last position we have to check
            if (!diff) {
                u32 len = ZSTD_count(in_next, matchptr, iLimit);
                if (len > best_len) {
                    best_len = len;
                    best_matchptr = matchptr;
                }
            }
            goto out; 
        }
        
        if (diff) continue;

        // Match passed quick checks; count exact length
        u32 len = ZSTD_count(in_next, matchptr, iLimit);
        if (len > best_len) { // Found better match
            best_len = len;
            best_matchptr = matchptr;
            i32_end = MEM_read32(in_next + best_len - 3);
            
            u32 const jump_node = mf->next4_tab[(match_pos + len - base_val) & HC_NEXT4_TAB_MASK];
            if (cur_node4 > jump_node) { // chain switching
                cur_node4 = jump_node;
                minus = len - base_val;
                bloom_filter_bit = 1 << (in_next[len - 1] & 7);
            }
        } else if ((len >= base_val)) { 
            u32 const new_minus = len - base_val;
            u32 const jump_node = mf->next4_tab[(match_pos + new_minus) & HC_NEXT4_TAB_MASK];
            if (new_minus != minus && cur_node4 > jump_node) {
                cur_node4 = jump_node;
                minus = new_minus;
                bloom_filter_bit = 1 << (in_next[len - 1] & 7);
            }
        }
    }
out:
    if (best_matchptr == 0) return 0;
    
    size_t off = (size_t)(in_next - best_matchptr);
    *offset_ret = STORE_OFFSET(off);
    return best_len;
}

static inline __attribute__((always_inline, hot)) size_t ZSTD_WRC_HcFindBestMatch(
    struct hc_matchfinder *const restrict mf, const BYTE *our_base, const BYTE *ip,
    const BYTE *const iLimit, size_t *offsetPtr, u32 bestLen, u32 our_windowLow, const int base_val)
{
#if defined(COLLECT_STATS) && (COLLECT_STATS >= 2)
    calls++;
    uint64_t pom = rdtsc();
    iter4 = 0;
#endif

    size_t ret = WRC_HcMatchfinderLongestMatch(mf, our_base, (u8*)ip, 
                     bestLen < (u32)base_val ? (base_val - 1) : bestLen, 
                     offsetPtr, iLimit, our_windowLow, base_val);

#if defined(COLLECT_STATS) && (COLLECT_STATS >= 2)
    int stat_idx = ret < (MAX_MATCH_STATS - 1) ? (int)ret : (MAX_MATCH_STATS - 1);
    tim[stat_idx] += rdtsc() - pom;
    cntt[stat_idx]++;
    iterr4[stat_idx] += iter4;
#endif
    return ret;
}

/*
 * Analyzes compressability of the input to suggest optimal minimum match length (5, 6, or 7).
 */
static inline int predict_min_match(const uint8_t* restrict src, size_t srcSize, uint32_t *hash_table) {
    if (srcSize < 1024) return 6;

    size_t sample_size = (srcSize < 57600) ? srcSize : 57600;
    size_t match_bytes = 0;
    size_t match_count = 0;
    const int HASHTABLE_SIZE = 14;

    memset(hash_table, 0, (1 << HASHTABLE_SIZE) * sizeof(uint32_t));

    for (size_t i = 0; i < sample_size - 8; i += 4) {
        uint32_t val = MEM_read32(src + i);
        uint32_t h = (val * 0x9E3779B1) >> (32 - HASHTABLE_SIZE);
        uint32_t prev = hash_table[h];
        hash_table[h] = (uint32_t)i;

        if (prev != 0 && MEM_read32(src + prev) == val) {
            size_t len = 4 + (uint32_t)ZSTD_count(src + i + 4, src + prev + 4,  src + srcSize); 
            match_bytes += len;
            match_count++;
            i += (len > 4) ? (len - 1) : 0;
        }
    }
    memset(hash_table, 0, (1 << HASHTABLE_SIZE) * sizeof(uint32_t));

    if (match_count == 0) {
        return 5;
    }

    uint32_t savings_pct = (uint32_t)(match_bytes * 100 / sample_size);
    uint32_t avg_len     = (uint32_t)(match_bytes / match_count);

    size_t compressed_estimate = (sample_size - match_bytes) + 3 * match_count;
    if (compressed_estimate < 1) compressed_estimate = 1;
    float cr = (float)sample_size / compressed_estimate;

    if (cr > 6){return 8;}
    if (savings_pct > 60 || avg_len > 13) return 7;
    if (savings_pct > 40 || avg_len > 8) return 6;
    return 5;
}

/*
 * Core LZ77 compression loop
 */
static inline __attribute__((always_inline)) size_t ZSTD_compressBlock_WRC_internal(
    ZSTD_MatchState_t *ms, SeqStore_t *seqStore,
    U32 rep[ZSTD_REP_NUM], const void *src, size_t srcSize, const int base_val,
    const BYTE* our_base, const BYTE* match_src)
{
    const BYTE *const base = ms->window.base;
    U32 const curr = (U32)((const BYTE *)src - base);
    U32 const windowLow = ZSTD_getLowestPrefixIndex(ms, curr, ms->cParams.windowLog);
    struct hc_matchfinder *WRC_mf = (struct hc_matchfinder *)ms->WRC_matchfinder;

    U32 const our_pos = curr - WRC_mf->stream_start_curr;
    U32 const max_dist = curr - windowLow;
    U32 const our_windowLow = (max_dist > our_pos) ? 0 : (our_pos - max_dist);

    // Update hash table with new block
    WRC_HcUpdateWindow(WRC_mf, our_base, match_src, srcSize, base_val);

    const BYTE *const istart = match_src;
    const BYTE *ip = istart;
    const BYTE *anchor = istart;
    const BYTE *const iend = istart + srcSize;
    const BYTE *const ilimit = iend - 8;

    if (our_pos == 0) ip++;

    U32 offset_1 = rep[0], offset_2 = rep[1];
    U32 offsetSaved1 = 0, offsetSaved2 = 0;
    U32 maxRep = curr - windowLow; 

    /* Check against history accesses not maintained within ext_dict_buffer boundaries */
    if (our_base == WRC_mf->ext_dict_buffer) {
        if (maxRep > our_pos) maxRep = our_pos;
    }

    if (offset_2 > maxRep) { offsetSaved2 = offset_2; offset_2 = 0; }
    if (offset_1 > maxRep) { offsetSaved1 = offset_1; offset_1 = 0; }
    
    // Main compression loop
    while (ip < ilimit) {
        const int idx = our_pos + (ip - istart);
        const U32 stepShift = (kSearchStrength - 1);
        size_t matchLength;
        size_t offcode;
        const BYTE *start;

        do {
            matchLength = 1;
            offcode = REPCODE1_TO_OFFBASE;
            start = ip + 1;

            // Check Repcode 1 (Cached previous offset)
            if (likely(offset_1 > 0) && MEM_read32(ip + 1 - offset_1) == MEM_read32(ip + 1)) {
                matchLength = ZSTD_count(ip + 1 + 4, ip + 1 + 4 - offset_1, iend) + 4;
                if (matchLength >= (size_t)base_val) {
                    goto use;
                }
            }

            size_t offsetFound = 0;
            size_t const ml2 = ZSTD_WRC_HcFindBestMatch(WRC_mf, our_base, ip, iend, &offsetFound, (int)matchLength, our_windowLow, base_val);
            
            if (ml2 > matchLength) {
                matchLength = ml2;
                start = ip;
                offcode = offsetFound;
                break;
            }
            if (matchLength >= 4) break;
            
            ip += ((size_t)(ip - anchor) >> stepShift) + 1;
        } while (ip < ilimit);

        if (unlikely(ip >= ilimit)) break;

use:
        if(STORED_IS_OFFSET(offcode)) {
            const size_t offset = STORED_OFFSET(offcode);
            const BYTE* mStart = start - offset;
            const BYTE* const history_low = our_base + our_windowLow;
            const BYTE* const lower = (anchor > history_low + offset) ? anchor : history_low + offset;

            while (start > lower && start[-1] == mStart[-1]) {
                start--;
                mStart--;
                matchLength++;
            }
            offset_2 = offset_1;
            offset_1 = (U32)offset;
        }

        // Store Sequence
        size_t const litLength = (size_t)(start - anchor);
        ZSTD_storeSeq(seqStore, litLength, anchor, iend, (U32)offcode, matchLength);
        anchor = ip = start + matchLength;
        
        // Continuous Repcode 2 evaluation
        while (offset_2 > 0 && ip <= ilimit && MEM_read32(ip) == MEM_read32(ip - offset_2)) {
            matchLength = ZSTD_count(ip + 4, ip + 4 - offset_2, iend) + 4;
            U32 tmp = offset_2;
            offset_2 = offset_1;
            offset_1 = tmp; 
            
            ZSTD_storeSeq(seqStore, 0, anchor, iend, REPCODE1_TO_OFFBASE, matchLength);
            ip += matchLength;
            anchor = ip;
        }
    }

    // Restore unused offsets
    offsetSaved2 = ((offsetSaved1 != 0) && (offset_1 != 0)) ? offsetSaved1 : offsetSaved2;
    rep[0] = offset_1 ? offset_1 : offsetSaved1;
    rep[1] = offset_2 ? offset_2 : offsetSaved2;

    return (size_t)(iend - anchor);
}

/*
 * Decrements positions in hash tables using ARM NEON to slide the compression window.
 * Avoids integer overflows on long streams.
 */
static void WRC_UpdateTable_SIMD(uint32_t* tab, uint32_t size, uint32_t shiftAmount) {
    const uint32_t shiftVal = shiftAmount << BITS_H;
    const uint32_t threshold = (shiftAmount << BITS_H) | 255;
    
    uint32x4_t v_shift = vdupq_n_u32(shiftVal);
    uint32x4_t v_thresh = vdupq_n_u32(threshold);
    
    uint32_t i = 0;
    for (; i + 15 < size; i += 16) {
        uint32x4_t v0 = vld1q_u32(tab + i);
        uint32x4_t v1 = vld1q_u32(tab + i + 4);
        uint32x4_t v2 = vld1q_u32(tab + i + 8);
        uint32x4_t v3 = vld1q_u32(tab + i + 12);

        uint32x4_t m0 = vcgtq_u32(v0, v_thresh);
        uint32x4_t m1 = vcgtq_u32(v1, v_thresh);
        uint32x4_t m2 = vcgtq_u32(v2, v_thresh);
        uint32x4_t m3 = vcgtq_u32(v3, v_thresh);

        vst1q_u32(tab + i,      vandq_u32(vsubq_u32(v0, v_shift), m0));
        vst1q_u32(tab + i + 4,  vandq_u32(vsubq_u32(v1, v_shift), m1));
        vst1q_u32(tab + i + 8,  vandq_u32(vsubq_u32(v2, v_shift), m2));
        vst1q_u32(tab + i + 12, vandq_u32(vsubq_u32(v3, v_shift), m3));
    }
    
    for (; i < size; i++) {
        uint32_t val = tab[i];
        if (val > threshold) tab[i] = val - shiftVal;
        else tab[i] = 0;
    }
}

static void WRC_HcShift(struct hc_matchfinder* mf, u32 leaveAfterShift)
{
    const u32 shiftAmount = MATCHFINDER_WINDOW_SIZE - leaveAfterShift; 
    const u32 retainSize = leaveAfterShift; // FIX: Never truncate bounds or it uninitializes valid gap gaps
    
    assert(shiftAmount % HC_NEXT4_TAB_SIZE == 0);
    
    WRC_UpdateTable_SIMD(mf->next4_tab, HC_NEXT4_TAB_SIZE, shiftAmount);
    WRC_UpdateTable_SIMD(mf->hash4_tab, (1 << (HC_MATCHFINDER_HASH4_ORDER - mf->high_cr * HIGH_CR_HASH_DISCOUNT)), shiftAmount);
    
    if (mf->ext_dict_used) {
        if (retainSize > 0 && shiftAmount > 0) {
            memmove(mf->ext_dict_buffer, mf->ext_dict_buffer + shiftAmount, retainSize * sizeof(uint8_t));
        }
    }
    
    mf->stream_start_curr += shiftAmount;
}
/*
 * Entry point for ZSTD Compressor.
 */
size_t ZSTD_compressBlock_WRC(ZSTD_MatchState_t *ms, SeqStore_t *seqStore,
    U32 rep[ZSTD_REP_NUM], const void *src, size_t srcSize, ZSTD_dictMode_e const dictMode)
{
    const BYTE *const base = ms->window.base;
    U32 const curr      = (U32)((const BYTE *)src - base);
    U32 const windowLow = ZSTD_getLowestPrefixIndex(ms, curr, ms->cParams.windowLog);
    static U32 small_block = 0;

    /* Lazy-allocate the matchfinder on first use for this ZSTD_MatchState_t. */
    if (unlikely(ms->WRC_matchfinder == NULL)) {
        ms->WRC_matchfinder = WRC_get_matchfinder();
        if (ms->WRC_matchfinder == NULL) return ERROR(memory_allocation);
    }
    struct hc_matchfinder* WRC_mf = (struct hc_matchfinder*)ms->WRC_matchfinder;
    if (small_block == 1 && curr != 2) {
        memset(WRC_mf->hash4_small_tab, 0, sizeof(WRC_mf->hash4_small_tab));
        return ZSTD_small_block_compressBlock_WRC_internal(ms, seqStore, rep, src, srcSize, 5, src, (const BYTE*)src);
    }

    /* ── 1. RESET ── first block, discontinuity, or no-history signal ── */
    bool const need_reset = !WRC_mf->initialized
                         || (curr != WRC_mf->last_curr_end)
                         || (dictMode == ZSTD_noDict && windowLow == curr);

    if (unlikely(need_reset)) {
        WRC_mf->initialized   = 1;
        WRC_mf->ext_dict_used = 0;
        WRC_mf->last_curr_end = curr;

        if ((curr == 2 && ms->compMode != ZSTDcs_Stream && ms->compressionSize <= ZSTD_BLOCKSIZE_MAX) ||
            (curr == 2 && ms->compMode == ZSTDcs_Stream && ms->compressionSize < ZSTD_BLOCKSIZE_MAX)) {
            small_block = 1;
            memset(WRC_mf->hash4_small_tab, 0, sizeof(WRC_mf->hash4_small_tab));
            return ZSTD_small_block_compressBlock_WRC_internal(ms, seqStore, rep, src, srcSize, 5, src, (const BYTE*)src);
        } else {
            small_block = 0;

            /* predict_min_match scratches hash4_tab; we zero it right after. */
            WRC_mf->base_val = predict_min_match(src, srcSize, WRC_mf->hash4_tab);
            WRC_mf->high_cr  = (WRC_mf->base_val == 8) ? 1 : 0;
            memset(WRC_mf->hash4_tab, 0,
                (1u << (HC_MATCHFINDER_HASH4_ORDER - WRC_mf->high_cr * HIGH_CR_HASH_DISCOUNT))
                * sizeof(mf_pos_t));
        }

        U32 dict_reserve = 0;
        if (dictMode == ZSTD_extDict) {
            U32 const dict_size   = ms->window.dictLimit - ms->window.lowLimit;
            U32 const prefix_size = (curr >= ms->window.dictLimit) ? (curr - ms->window.dictLimit) : 0;
            U32 const max_dict    = MATCHFINDER_WINDOW_SIZE - (U32)srcSize - LEAVE_AFTER_SHIFT - 16;
            U32 const total_hist  = dict_size + prefix_size;
            dict_reserve = (total_hist > max_dict) ? max_dict : total_hist;
            if (dict_reserve > curr) dict_reserve = curr; /* avoid u32 underflow */
        }
        WRC_mf->stream_start_curr = curr - dict_reserve;
    }

    /* ── 2. SHIFT ── slide window forward if it'd overflow ── */
    if ((curr - WRC_mf->stream_start_curr) + (U32)srcSize + 8 >= MATCHFINDER_WINDOW_SIZE) {
        WRC_HcShift(WRC_mf, LEAVE_AFTER_SHIFT);
    }

    U32 const our_pos = curr - WRC_mf->stream_start_curr;

    if (dictMode == ZSTD_extDict && !WRC_mf->ext_dict_used) {
        WRC_mf->ext_dict_used = 1;

        U32 const dict_size   = ms->window.dictLimit - ms->window.lowLimit;
        U32 const prefix_size = (curr >= ms->window.dictLimit) ? (curr - ms->window.dictLimit) : 0;

        /* Total bytes copied are bounded by our_pos (room reserved in Stage 1).
         * Prefix is the more recent history, so it sits closer to our_pos. */
        U32 const copy_prefix = (prefix_size > our_pos) ? our_pos : prefix_size;
        U32 const room_left   = our_pos - copy_prefix;
        U32 const copy_dict   = (room_left > dict_size) ? dict_size : room_left;

        /* Splitting history copies guarantees valid contiguous prefix logic */
        if (copy_prefix > 0) {
            memcpy(WRC_mf->ext_dict_buffer + our_pos - copy_prefix,
                   ms->window.base + curr - copy_prefix,
                   copy_prefix);
        }
        if (copy_dict > 0) {
            memcpy(WRC_mf->ext_dict_buffer + our_pos - copy_prefix - copy_dict,
                   ms->window.dictBase + ms->window.dictLimit - copy_dict,
                   copy_dict);
        }
    }

    /* ── 4. STAGE INPUT ── pick our_base + match_src ── */
    const BYTE* our_base;
    const BYTE* match_src;
    if (WRC_mf->ext_dict_used) {
        memcpy(WRC_mf->ext_dict_buffer + our_pos, src, srcSize);
        our_base  = WRC_mf->ext_dict_buffer;
        match_src = WRC_mf->ext_dict_buffer + our_pos;
    } else {
        our_base  = (const BYTE*)src - our_pos;
        match_src = (const BYTE*)src;
    }

    WRC_mf->last_curr_end = curr + (U32)srcSize;

    /* ── 5. DISPATCH ── LZ77 inner loop, specialized on base_val ── */
    switch (WRC_mf->base_val) {
        case 5:  return ZSTD_compressBlock_WRC_internal(ms, seqStore, rep, src, srcSize, 5, our_base, match_src);
        case 6:  return ZSTD_compressBlock_WRC_internal(ms, seqStore, rep, src, srcSize, 6, our_base, match_src);
        case 7:  return ZSTD_compressBlock_WRC_internal(ms, seqStore, rep, src, srcSize, 7, our_base, match_src);
        default: return ZSTD_compressBlock_WRC_internal(ms, seqStore, rep, src, srcSize, 8, our_base, match_src);
    }
}

size_t ZSTD_compressBlock_WRC_no_dict(ZSTD_MatchState_t *ms, SeqStore_t *seqStore,
                              U32 rep[ZSTD_REP_NUM], const void *src, size_t srcSize) {
    return ZSTD_compressBlock_WRC(ms, seqStore, rep, src, srcSize, ZSTD_noDict);
}

size_t ZSTD_compressBlock_WRC_ext_dict(ZSTD_MatchState_t *ms, SeqStore_t *seqStore,
                              U32 rep[ZSTD_REP_NUM], const void *src, size_t srcSize) {
    return ZSTD_compressBlock_WRC(ms, seqStore, rep, src, srcSize, ZSTD_extDict);
}

/* 
 * WRC_reduceIndex — track ZSTD's index reduction
 */
void WRC_reduceIndex(void* opaque_mf, U32 reducerValue) {
    if (!opaque_mf) return;
    struct hc_matchfinder* mf = (struct hc_matchfinder*)opaque_mf;
    if (!mf->initialized) return;

    mf->stream_start_curr -= reducerValue;
    mf->last_curr_end     -= reducerValue;
}