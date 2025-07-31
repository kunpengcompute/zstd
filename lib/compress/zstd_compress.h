#include "../common/allocations.h"
#include "../common/zstd_deps.h"
#include "../common/mem.h"
#include "hist.h"
#include "../common/fse.h"
#include "../common/huf.h"
#include "zstd_compress_internal.h"
#include "zstd_compress_sequences.h"
#include "zstd_compress_literals.h"
#include "zstd_fast.h"
#include "zstd_double_fast.h"
#include "zstd_lazy.h"
#include "zstd_opt.h"
#include "zstd_ldm.h"
#include "zstd_compress_superblock.h"
#ifndef BPSF_ZSTD_COMPRESS_H
#define BPSF_ZSTD_COMPRESS_H

typedef struct {
    U32 LLtype;
    U32 Offtype;
    U32 MLtype;
    size_t size;
    size_t lastCountSize;
    int longOffsets;
} ZSTD_symbolEncodingTypeStats_t;

ZSTD_parameters ZSTD_getParams_internal(int compressionLevel, unsigned long long srcSizeHint, size_t dictSize, ZSTD_cParamMode_e mode);

size_t ZSTD_buildSeqStore(ZSTD_CCtx* zc, const void* src, size_t srcSize);

size_t ZSTD_compressBegin_internal(ZSTD_CCtx* cctx, const void* dict, size_t dictSize, ZSTD_dictContentType_e dictContentType, ZSTD_dictTableLoadMethod_e dtlm,
                                    const ZSTD_CDict* cdict, const ZSTD_CCtx_params* params, U64 pledgedSrcSize, ZSTD_buffered_policy_e zbuff);

void ZSTD_CCtxParams_init_internal(ZSTD_CCtx_params* cctxParams, const ZSTD_parameters* params, int compressionLevel);

ZSTD_symbolEncodingTypeStats_t ZSTD_buildSequencesStatistics(const seqStore_t* seqStorePtr, size_t nbSeq, const ZSTD_fseCTables_t* prevEntropy,
                                                            ZSTD_fseCTables_t* nextEntropy, BYTE* dst, const BYTE* const dstEnd, ZSTD_strategy strategy, 
                                                            unsigned* countWorkspace, void* entropyWorkspace, size_t entropyWkspSize);

size_t HUF_load_table(const BYTE* src, HUF_DTable* dtable);

size_t HUF_build_table(BYTE* dst, size_t dst_capacity, const BYTE* src, size_t srcSize, HUF_CElt* ctable);
#endif // BPSF_ZSTD_COMPRESS_H