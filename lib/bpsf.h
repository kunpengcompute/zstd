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

void BPSF_compressBegin(ZSTD_CCtx* cctx,
                        const void* dict, size_t dictSize,
                        ZSTD_dictContentType_e dictContentType,
                        ZSTD_dictTableLoadMethod_e dtlm,
                        const ZSTD_CDict* cdict,
                        const ZSTD_CCtx_params* params, U64 pledgedSrcSize,
                        ZSTD_buffered_policy_e zbuff);
    
void BPSF_getSeqStore(ZSTD_CCtx* zc, const void* src, size_t srcSize);

U32 BPSF_update_window(ZSTD_window_t* window, void const* src, size_t srcSize, int forceNonContiguous);

// Huffman
size_t BPSF_build_HUFTable(BYTE* dst, size_t dst_capacity, const BYTE* src, size_t srcSize, HUF_CElt* CTable);

size_t BPSF_loadHUFTable(const BYTE* src, HUF_DTable* dtable);

// FSE encode
ZSTD_symbolEncodingTypeStats_t BPSF_buildSeqsStats(
        const seqStore_t *seqStorePtr, size_t nbSeq,
        const ZSTD_fseCTables_t *prevEntropy, ZSTD_fseCTables_t *nextEntropy,
        BYTE *dst, const BYTE *const dstEnd,
        ZSTD_strategy strategy, unsigned *countWorkspace,
        void *entropyWorkspace, size_t entropyWkspSize
);

size_t BPSF_encodeSeqs(
        void* dst, size_t dstCapacity,
        FSE_CTable const* CTable_MatchLength, BYTE const* mlCodeTable,
        FSE_CTable const* CTable_OffsetBits, BYTE const* ofCodeTable,
        FSE_CTable const* CTable_LitLength, BYTE const* llCodeTable,
        seqDef const* sequences, size_t nbSeq, int longOffsets, int bmi2
);

// FSE decode
size_t BPSF_decodeSeqTable(ZSTD_DCtx* dctx, size_t n_seq, const BYTE* p_src);

size_t BPSF_decodeSeqs(
    ZSTD_DCtx* dctx, const void* seqStart, size_t seqSize, int nbSeq,
    uint16_t *p_ll, uint16_t *p_ml, uint16_t *p_of
);

#endif // BPSF_BPSF_H