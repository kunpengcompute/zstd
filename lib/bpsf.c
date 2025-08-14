/*
* 版权所有 (c) 华为技术有限公司 2025
*/

#include "bpsf.h"
#include "zstd.h"
#include "compress/zstd_compress.h"
#include "common/zstd_internal.h"
#include "decompress/zstd_decompress_internal.h"

#include "decompress/zstd_decompress_block.h"
#include "mem.h"

ZSTD_CCtx *BPSF_getCCtx(void) {
    return ZSTD_createCCtx();
}

size_t BPSF_freeCCtx(ZSTD_CCtx *p_ctx) {
    return ZSTD_freeCCtx(p_ctx);
}

ZSTD_DCtx *BPSF_getDCtx(void) {
    return ZSTD_createDCtx();
}

size_t BPSF_freeDCtx(ZSTD_DCtx *p_ctx) {
    return ZSTD_freeDCtx(p_ctx);
}

ZSTD_parameters
BPSF_getParams(int compressionLevel, unsigned long long srcSizeHint, size_t dictSize, ZSTD_cParamMode_e mode) {
    return ZSTD_getParams_internal(compressionLevel, srcSizeHint, dictSize, mode);
}

void BPSF_init_CCtxParams(ZSTD_CCtx_params *cctxParams, const ZSTD_parameters *params, int compressionLevel) {
    return ZSTD_CCtxParams_init_internal(cctxParams, params, compressionLevel);
}

void BPSF_compressBegin(ZSTD_CCtx *cctx, const uint8_t* dict, size_t dictSize, ZSTD_dictContentType_e dictContentType,
                        ZSTD_dictTableLoadMethod_e dtlm, const ZSTD_CDict *cdict, const ZSTD_CCtx_params *params,
                        U64 pledgedSrcSize, ZSTD_buffered_policy_e zbuff) {
    ZSTD_compressBegin_internal(cctx, dict, dictSize, dictContentType, dtlm, cdict, params, pledgedSrcSize, zbuff);
}

void BPSF_getSeqStore(ZSTD_CCtx *zc, const uint8_t* src, size_t srcSize) {
    ZSTD_buildSeqStore(zc, src, srcSize);
}

U32 BPSF_update_window(ZSTD_window_t *window, const uint8_t* src, size_t srcSize, int forceNonContiguous) {
    return ZSTD_window_update(window, src, srcSize, forceNonContiguous);
}

size_t BPSF_build_HUFTable(BYTE *dst, size_t dst_capacity, const BYTE *src, size_t srcSize, HUF_CElt *CTable) {
    return HUF_build_table(dst, dst_capacity, src, srcSize, CTable);
}

size_t BPSF_loadHUFTable(const BYTE *src, HUF_DTable *dtable) {
    return HUF_load_table(src, dtable);
}

ZSTD_symbolEncodingTypeStats_t BPSF_buildSeqsStats(const seqStore_t *seqStorePtr, size_t nbSeq, const ZSTD_fseCTables_t *prevEntropy, 
                                                   ZSTD_fseCTables_t *nextEntropy, uint8_t *dst, const uint8_t* dstEnd, ZSTD_strategy strategy, 
                                                   unsigned *countWorkspace, uint8_t *entropyWorkspace, size_t entropyWkspSize) {
    return ZSTD_buildSequencesStatistics(seqStorePtr, nbSeq, prevEntropy, nextEntropy, dst, dstEnd,
                                         strategy, countWorkspace, entropyWorkspace, entropyWkspSize);
}



size_t BPSF_encodeSeqs(uint8_t *dst, size_t dstCapacity, const FSE_CTable* CTable_MatchLength, 
                       const uint8_t* mlCodeTable, const FSE_CTable* CTable_OffsetBits, const uint8_t* ofCodeTable,
                       const FSE_CTable* CTable_LitLength, const uint8_t* llCodeTable, const seqDef* sequences, 
                       size_t nbSeq, int longOffsets, int bmi2) {
    return ZSTD_encodeSequences(dst, dstCapacity, CTable_MatchLength, mlCodeTable, CTable_OffsetBits, 
                                ofCodeTable, CTable_LitLength, llCodeTable, sequences, 
                                nbSeq, longOffsets, bmi2);
}

extern size_t ZSTD_buildSeqTable(ZSTD_seqSymbol* DTableSpace, const ZSTD_seqSymbol** DTablePtr,
                                 symbolEncodingType_e type, unsigned max, U32 maxLog, 
                                 const void* src, size_t srcSize,
                                 const U32* baseValue, const U8* nbAdditionalBits,
                                 const ZSTD_seqSymbol* defaultTable, U32 flagRepeatTable,
                                 int ddictIsCold, int nbSeq, U32* wksp, size_t wkspSize, 
                                 int bmi2);

extern const ZSTD_seqSymbol LL_defaultDTable[(1<<LL_DEFAULTNORMLOG)+1];
extern const ZSTD_seqSymbol OF_defaultDTable[(1<<OF_DEFAULTNORMLOG)+1];
extern const ZSTD_seqSymbol ML_defaultDTable[(1<<ML_DEFAULTNORMLOG)+1];

size_t BPSF_decodeSeqTable(ZSTD_DCtx* dctx, size_t n_seq, const BYTE* p_src) {
    const size_t src_size = 1024;
    const BYTE* p_src_end = p_src + src_size;

    symbolEncodingType_e const LLtype = (symbolEncodingType_e)(*p_src >> 6);
    symbolEncodingType_e const OFtype = (symbolEncodingType_e)((*p_src >> 4) & 3);
    symbolEncodingType_e const MLtype = (symbolEncodingType_e)((*p_src >> 2) & 3);
    p_src++;


    {
        size_t const llhSize = ZSTD_buildSeqTable(dctx->entropy.LLTable, &dctx->LLTptr,
                                                  LLtype, MaxLL, LLFSELog,
                                                  p_src, p_src_end-p_src,
                                                  LL_base, LL_bits,
                                                  LL_defaultDTable, dctx->fseEntropy,
                                                  dctx->ddictIsCold, n_seq,
                                                  dctx->workspace, sizeof(dctx->workspace), 
                                                  0);
        RETURN_ERROR_IF(ZSTD_isError(llhSize), corruption_detected, "ZSTD_buildSeqTable failed");
        p_src += llhSize;
    }

    {
        size_t const ofhSize = ZSTD_buildSeqTable(dctx->entropy.OFTable, &dctx->OFTptr,
                                                  OFtype, MaxOff, OffFSELog,
                                                  p_src, p_src_end-p_src,
                                                  OF_base, OF_bits,
                                                  OF_defaultDTable, dctx->fseEntropy,
                                                  dctx->ddictIsCold, n_seq,
                                                  dctx->workspace, sizeof(dctx->workspace), 
                                                  0);
        RETURN_ERROR_IF(ZSTD_isError(ofhSize), corruption_detected, "ZSTD_buildSeqTable failed");
        p_src += ofhSize;
    }

    {
        size_t const mlhSize = ZSTD_buildSeqTable(dctx->entropy.MLTable, &dctx->MLTptr,
                                                  MLtype, MaxML, MLFSELog,
                                                  p_src, p_src_end-p_src,
                                                  ML_base, ML_bits,
                                                  ML_defaultDTable, dctx->fseEntropy,
                                                  dctx->ddictIsCold, n_seq,
                                                  dctx->workspace, sizeof(dctx->workspace), 
                                                  0);
        RETURN_ERROR_IF(ZSTD_isError(mlhSize), corruption_detected, "ZSTD_buildSeqTable failed");
        p_src += mlhSize;
    }

    return 0;
}

typedef struct {
    size_t state;
    const ZSTD_seqSymbol* table;
} ZSTD_fseState;

typedef struct {
    BIT_DStream_t DStream;
    ZSTD_fseState stateLL;
    ZSTD_fseState stateOffb;
    ZSTD_fseState stateML;
    size_t prevOffset[ZSTD_REP_NUM];
} seqState_t;

typedef struct {
    size_t litLength;
    size_t matchLength;
    size_t offset;
} seq_t;

typedef enum { ZSTD_lo_isRegularOffset, ZSTD_lo_isLongOffset=1 } ZSTD_longOffset_e;

extern void ZSTD_initFseState(ZSTD_fseState* DStatePtr, BIT_DStream_t* bitD, const ZSTD_seqSymbol* dt);
extern seq_t ZSTD_decodeSequence(seqState_t* seqState, const ZSTD_longOffset_e longOffsets, const int isLastSeq);

size_t BPSF_decodeSeqs (ZSTD_DCtx* dctx, const uint8_t* seqStart, size_t seqSize, 
                        int nbSeq, uint16_t *p_ll, uint16_t *p_ml, uint16_t *p_of) {
    const BYTE* ip = (const BYTE*)seqStart;
    const BYTE* const iend = ip + seqSize;

    size_t i_seq = 0;
    if (nbSeq) {
        seqState_t seqState;
        dctx->fseEntropy = 1;
        seqState.prevOffset[0] = 1;
        seqState.prevOffset[1] = 4;
        seqState.prevOffset[2] = 8;

        RETURN_ERROR_IF(ERR_isError(BIT_initDStream(&seqState.DStream, ip, iend-ip)), corruption_detected, "");
        ZSTD_initFseState(&seqState.stateLL, &seqState.DStream, dctx->LLTptr);
        ZSTD_initFseState(&seqState.stateOffb, &seqState.DStream, dctx->OFTptr);
        ZSTD_initFseState(&seqState.stateML, &seqState.DStream, dctx->MLTptr);

        for ( ; nbSeq ; nbSeq--) {
            seq_t const sequence = ZSTD_decodeSequence(&seqState, 0, nbSeq==1);
            p_ll[i_seq] = sequence.litLength;
            p_ml[i_seq] = sequence.matchLength;
            p_of[i_seq] = sequence.offset;
            i_seq ++;
        }


        assert(nbSeq == 0);
        RETURN_ERROR_IF(!BIT_endOfDStream(&seqState.DStream), corruption_detected, "");
    }
    
    return 0;
}

size_t BPSF_decodeSeqs_and_reconstruct(ZSTD_DCtx* dctx, const uint8_t* seqStart, size_t seqSize,
                                        int nbSeq, uint8_t *p_dst, size_t max_dst_len, 
                                        size_t dict_size, size_t *reconstructed_size) {
    const BYTE *p_dict = dctx->litPtr;
    const BYTE *p_dict_end = dctx->litPtr + dict_size;

    const BYTE *p_lit = dctx->litPtr + dict_size;
    const BYTE *p_lit_end = dctx->litPtr + dctx->litSize;

    BYTE *p_dst_start = p_dst;
    BYTE *p_dst_limit = p_dst + max_dst_len;

    uint8_t backup [16];
    MEM_COPY16B(backup, p_dst_limit);

    S32 ll_state, of_state, ml_state;
    U64 data;

    const BYTE* p_src = (const BYTE*)seqStart;
    size_t src_len = seqSize;

    S32 prev_of[] = {1, 4, 8};

    p_src += (src_len - 8);

    #define FSE_READMOVE0(t,b,n) { if(n) { t=b+(data>>(64-n)); data<<=n;  } else {t=b;} }
    #define FSE_READMOVE1(t,b,n)  {         t=b+(data>>(64-n)); data<<=n; } 

    if (nbSeq) {
        dctx->fseEntropy = 1;

        U8 ll_m_bits = ((const ZSTD_seqSymbol_header*)dctx->LLTptr)->tableLog;
        U8 of_m_bits = ((const ZSTD_seqSymbol_header*)dctx->OFTptr)->tableLog;
        U8 ml_m_bits = ((const ZSTD_seqSymbol_header*)dctx->MLTptr)->tableLog;

        const ZSTD_seqSymbol* ll_table = (dctx->LLTptr + 1);
        const ZSTD_seqSymbol* of_table = (dctx->OFTptr + 1);
        const ZSTD_seqSymbol* ml_table = (dctx->MLTptr + 1);

        data = (1 | (*(U64*)p_src));
        data <<= (8 - highbit_u9(p_src[7]));

        FSE_READMOVE0(ll_state, 0, ll_m_bits);
        FSE_READMOVE0(of_state, 0, of_m_bits);
        FSE_READMOVE0(ml_state, 0, ml_m_bits);

        for (int i_seq = 0; i_seq < nbSeq; ++i_seq) {
            ZSTD_seqSymbol ll_item = ll_table[ll_state];
            ZSTD_seqSymbol of_item = of_table[of_state];
            ZSTD_seqSymbol ml_item = ml_table[ml_state];
            S32 of, ml, ll;

            {
                int8_t c = trailbit_u64(data);
                p_src -= (c>>3);
                data = (1 | (*(U64*)p_src));
                data <<= (c&7);
            }

            if (of_item.nbAdditionalBits > 1) {
                FSE_READMOVE1(of, of_item.baseValue, of_item.nbAdditionalBits);
                prev_of[2] = prev_of[1];
                prev_of[1] = prev_of[0];
                prev_of[0] = of;
            } else {
                U8 ll0 = (ll_item.baseValue == 0);
                if (of_item.nbAdditionalBits == 0) {
                    of = prev_of[ll0];
                    prev_of[1] = prev_of[!ll0];
                    prev_of[0] = of;
                } else {
                    FSE_READMOVE1(of, (of_item.baseValue+ll0), 1);
                    size_t temp = (of==3) ? prev_of[0] -1 : prev_of[of];
                    temp -= !temp;
                    if (of != 1) prev_of[2] = prev_of[1];
                    prev_of[1] = prev_of[0];
                    prev_of[0] = of = temp;
                }
            }

            FSE_READMOVE0(ml, ml_item.baseValue, ml_item.nbAdditionalBits);
            FSE_READMOVE0(ll, ll_item.baseValue, ll_item.nbAdditionalBits);

            if (UNLIKELY(of_item.nbAdditionalBits + ml_item.nbAdditionalBits + ll_item.nbAdditionalBits > 30)) {
                int8_t c = trailbit_u64(data);
                p_src -= (c>>3);
                data   = (1 | (*(U64*)p_src));
                data <<= (c&7);
            }

            FSE_READMOVE0(ll_state, ll_item.nextState, ll_item.nbBits);
            FSE_READMOVE0(ml_state, ml_item.nextState, ml_item.nbBits);
            FSE_READMOVE0(of_state, of_item.nextState, of_item.nbBits);

            MEM_COPY16B(p_dst, p_lit);

            if (UNLIKELY(ll > 16)) {
                MEM_COPY(p_dst + 16, p_lit + 16, ll - 16);
            }
            p_dst += ll;
            p_lit += ll;
        

            if (of > p_dst - p_dst_start) {
                const U8 *dict_end = p_dict + dict_size;
                const U8 *dict_match = p_dict_end - (of - (p_dst - p_dst_start));
                if (dict_match + ml <= dict_end) {
                    ZSTD_wildcopy(p_dst, dict_match, ml, ZSTD_overlap_src_before_dst);
                } else {
                    size_t copy_from_dict = dict_end - dict_match;
                    ZSTD_wildcopy(p_dst, dict_match, copy_from_dict, ZSTD_overlap_src_before_dst);
                    ZSTD_wildcopy(p_dst + copy_from_dict, p_dst_start, ml - copy_from_dict, ZSTD_overlap_src_before_dst);
                }
            } else {
                const U8 *p_match = p_dst - of;
                if (LIKELY(of >= 16)) {
                    MEM_COPY(p_dst, p_match, ml);
                } else if (UNLIKELY(of == 4)) {
                    MEM_SET_4B(p_dst, *(uint32_t*)p_match, ml);
                } else if (UNLIKELY(of == 2)) {
                    MEM_SET_2B(p_dst, *(uint16_t*)p_match, ml);
                } else if (UNLIKELY(of == 1)) {
                    MEM_SET_1B(p_dst, *p_match, ml);
                } else {
                    U8 *op = p_dst;
                    ZSTD_overlapCopy8(&op, &p_match, of);
                    if (ml > 8) {
                        ZSTD_wildcopy(op, p_match, (ptrdiff_t)ml - 8, ZSTD_overlap_src_before_dst);
                    }
                }
            }
            p_dst += ml;
        }
    }

    {
        size_t n_last_lit = p_lit_end - p_lit;
        MEM_COPY(p_dst, p_lit, n_last_lit);
        p_dst += n_last_lit;
    }

    MEM_COPY16B(p_dst_limit, backup);
    *reconstructed_size = p_dst - p_dst_start;
    return 0;
}
