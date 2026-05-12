#pragma once
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;
typedef int s32;
typedef u32 mf_pos_t;
typedef unsigned char u8;

#define read32(memPtr) (MEM_read32(memPtr))
#define read40(memPtr) (MEM_read64(memPtr) & 0xFFFFFFFFFFULL)
#define read48(memPtr) (MEM_read64(memPtr) & 0xFFFFFFFFFFFFULL)
#define read56(memPtr) (MEM_read64(memPtr) & 0xFFFFFFFFFFFFFFULL)
#define read64(memPtr) (MEM_read64(memPtr))


#undef forceinline
#undef unlikely
#undef likely
#define forceinline __attribute__((always_inline))
#define likely(expr)   __builtin_expect(!!(expr), 1)
#define unlikely(expr) __builtin_expect(!!(expr), 0)

#define REPCODE1_TO_OFFBASE REPCODE_TO_OFFBASE(1)
#define REPCODE2_TO_OFFBASE REPCODE_TO_OFFBASE(2)
#define REPCODE3_TO_OFFBASE REPCODE_TO_OFFBASE(3)
#define REPCODE_TO_OFFBASE(r) (assert((r)>=1), assert((r)<=ZSTD_REP_NUM), (r)) 
#define OFFSET_TO_OFFBASE(o)  (assert((o)>0), o + ZSTD_REP_NUM) 
#define OFFBASE_IS_OFFSET(o)  ((o) > ZSTD_REP_NUM)
#define OFFBASE_IS_REPCODE(o) ( 1 <= (o) && (o) <= ZSTD_REP_NUM)
#define OFFBASE_TO_OFFSET(o)  (assert(OFFBASE_IS_OFFSET(o)), (o) - ZSTD_REP_NUM)
#define OFFBASE_TO_REPCODE(o) (assert(OFFBASE_IS_REPCODE(o)), (o))  
#define STORED_IS_OFFSET OFFBASE_IS_OFFSET
#define STORE_OFFSET OFFSET_TO_OFFBASE
#define STORED_OFFSET OFFBASE_TO_OFFSET
#define STORED_TO_OFFBASE(o) ((o)+1)