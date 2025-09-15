/*
* 版权所有 (c) 华为技术有限公司 2025
*/
#ifndef BPSF_BPSF_H
#define BPSF_BPSF_H

#include <stdint.h>
#include "zstd.h"
#include "compress/zstd_compress.h"

// LZ77
ZSTD_CCtx* BPSF_getCCtx(void);
size_t BPSF_freeCCtx(ZSTD_CCtx *p_cctx);
ZSTD_DCtx* BPSF_getDCtx(void);
size_t BPSF_freeDCtx(ZSTD_DCtx *p_cctx);

ZSTD_parameters BPSF_getParams(int compressionLevel, unsigned long long srcSizeHint, size_t dictSize, ZSTD_cParamMode_e mode);

void BPSF_init_CCtxParams(ZSTD_CCtx_params* cctxParams, const ZSTD_parameters* params, int compressionLevel);

void BPSF_compressBegin(ZSTD_CCtx* cctx, const uint8_t* dict, size_t dictSize, ZSTD_dictContentType_e dictContentType,
                        ZSTD_dictTableLoadMethod_e dtlm, const ZSTD_CDict* cdict, const ZSTD_CCtx_params* params, 
                        U64 pledgedSrcSize, ZSTD_buffered_policy_e zbuff, uint32_t *incrDictHashTable, size_t prevDictSize);
    
void BPSF_getSeqStore(ZSTD_CCtx* zc, const uint8_t* src, size_t srcSize);

U32 BPSF_update_window(ZSTD_window_t* window, const uint8_t* src, size_t srcSize, int forceNonContiguous);

// Huffman
size_t BPSF_build_HUFTable(uint8_t* dst, size_t dst_capacity, const uint8_t* src, size_t srcSize, HUF_CElt* CTable);

size_t BPSF_loadHUFTable(const uint8_t* src, HUF_DTable* dtable, size_t dtable_size);

size_t BPSF_loadHUFTable_X2(const uint8_t* src, HUF_DTable* dtable, size_t dtable_size);

// FSE encode
ZSTD_symbolEncodingTypeStats_t BPSF_buildSeqsStats(const seqStore_t *seqStorePtr, size_t nbSeq, const ZSTD_fseCTables_t *prevEntropy, 
                                                   ZSTD_fseCTables_t *nextEntropy, uint8_t *dst, const uint8_t* dstEnd, ZSTD_strategy strategy, 
                                                   unsigned *countWorkspace, uint8_t *entropyWorkspace, size_t entropyWkspSize);

size_t BPSF_encodeSeqs(uint8_t* dst, size_t dstCapacity,
        const FSE_CTable* CTable_MatchLength, const uint8_t* mlCodeTable,
        const FSE_CTable* CTable_OffsetBits, const uint8_t* ofCodeTable,
        const FSE_CTable* CTable_LitLength, const uint8_t* llCodeTable,
        const seqDef* sequences, size_t nbSeq, int longOffsets, int bmi2
);

// FSE decode
size_t BPSF_decodeSeqTable(ZSTD_DCtx* dctx, size_t n_seq, const uint8_t* p_src);

size_t BPSF_decodeSeqs(ZSTD_DCtx* dctx, const uint8_t* seqStart, size_t seqSize, 
                       int nbSeq, uint16_t *p_ll, uint16_t *p_ml, uint16_t *p_of);

void ZSTD_setLiteralDict(ZSTD_DCtx* dctx, const uint8_t* litPtr, size_t litSize);

size_t BPSF_decodeSeqs_and_reconstruct(ZSTD_DCtx* dctx, const uint8_t* seqStart, size_t seqSize,
                                       int nbSeq, uint8_t *p_dst, size_t max_dst_len, 
                                       size_t dict_size, size_t *reconstructed_size);
									   
void BPSF_hashReset(ZSTD_CCtx *cCtx);

void BPSF_hashUpdate(ZSTD_CCtx *cCtx, const uint8_t *src, size_t srcSize);

uint16_t BPSF_hashDigest(ZSTD_CCtx *cCtx);

#endif // BPSF_BPSF_H