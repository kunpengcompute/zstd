#include "bpsf.h"
#include "zstd.h"
#include "compress/zstd_compress.h"
#include "common/zstd_internal.h"
#include "decompress/zstd_decompress_internal.h"

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

void BPSF_compressBegin(ZSTD_CCtx *cctx, const void *dict, size_t dictSize, ZSTD_dictContentType_e dictContentType,
                        ZSTD_dictTableLoadMethod_e dtlm, const ZSTD_CDict *cdict, const ZSTD_CCtx_params *params,
                        U64 pledgedSrcSize, ZSTD_buffered_policy_e zbuff) {
    ZSTD_compressBegin_internal(cctx, dict, dictSize, dictContentType, dtlm, cdict, params, pledgedSrcSize, zbuff);
}

void BPSF_getSeqStore(ZSTD_CCtx *zc, const void *src, size_t srcSize) {
    ZSTD_buildSeqStore(zc, src, srcSize);
}

U32 BPSF_update_window(ZSTD_window_t *window, void const *src, size_t srcSize, int forceNonContiguous) {
    return ZSTD_window_update(window, src, srcSize, forceNonContiguous);
}

size_t BPSF_build_HUFTable(BYTE *dst, size_t dst_capacity, const BYTE *src, size_t srcSize, HUF_CElt *CTable) {
    return HUF_build_table(dst, dst_capacity, src, srcSize, CTable);
}

size_t BPSF_loadHUFTable(const BYTE *src, HUF_DTable *dtable) {
    return HUF_load_table(src, dtable);
}

ZSTD_symbolEncodingTypeStats_t BPSF_buildSeqsStats(
        const seqStore_t *seqStorePtr, size_t nbSeq,
        const ZSTD_fseCTables_t *prevEntropy, ZSTD_fseCTables_t *nextEntropy,
        BYTE *dst, const BYTE *const dstEnd,
        ZSTD_strategy strategy, unsigned *countWorkspace,
        void *entropyWorkspace, size_t entropyWkspSize) {
    return ZSTD_buildSequencesStatistics(seqStorePtr, nbSeq, prevEntropy, nextEntropy, dst, dstEnd,
                                         strategy, countWorkspace, entropyWorkspace, entropyWkspSize);
}



size_t BPSF_encodeSeqs(
        void *dst, size_t dstCapacity,
        FSE_CTable const *CTable_MatchLength, BYTE const *mlCodeTable,
        FSE_CTable const *CTable_OffsetBits, BYTE const *ofCodeTable,
        FSE_CTable const *CTable_LitLength, BYTE const *llCodeTable,
        seqDef const *sequences, size_t nbSeq, int longOffsets, int bmi2) {
    return ZSTD_encodeSequences(dst, dstCapacity, CTable_MatchLength, mlCodeTable, CTable_OffsetBits, 
                                ofCodeTable, CTable_LitLength, llCodeTable, sequences, 
                                nbSeq, longOffsets, bmi2);
}

extern size_t ZSTD_buildSeqTable(ZSTD_seqSymbol* DTableSpace, const ZSTD_seqSymbol** DTablePtr,
                                 symbolEncodingType_e type, unsigned max, U32 maxLog, 
                                 const void* src, size_t srcSize,
                                 const U32* baseValue, const U8* nbAdditionalBits,
                                 const ZSTD_seqSymbol* defaultTable, U32 flagRepeatTable,
                                 int ddictIsCold, int nbSeq, U32* wksp, size_t wkspSize, int bmi2);

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
        size_t const llhSize = ZSTD_buildSeqTable(
                            dctx->entropy.LLTable, &dctx->LLTptr,
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

size_t BPSF_decodeSeqs (
        ZSTD_DCtx* dctx, const void* seqStart, size_t seqSize, int nbSeq,
        uint16_t *p_ll, uint16_t *p_ml, uint16_t *p_of
) {
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