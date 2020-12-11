CC = gcc
CXX = g++

BUILD_TYPE = make
DEFINES += -DHAVE_CONFIG_H -DFL2_SINGLETHREAD
CODE_FLAGS += -Wno-unknown-pragmas -Wno-sign-compare -Wno-conversion
OPT_FLAGS ?= -fomit-frame-pointer -fstrict-aliasing -ffast-math
ifeq ($(BUILD_TYPE),debug)
    OPT_FLAGS_O2 = $(OPT_FLAGS) -g
    OPT_FLAGS_O3 = $(OPT_FLAGS) -g
else
    OPT_FLAGS_O2 = $(OPT_FLAGS) -O2 -DNDEBUG
    OPT_FLAGS_O3 = $(OPT_FLAGS) -O3 -DNDEBUG
endif

CFLAGS = -fPIC $(MOREFLAGS) $(CODE_FLAGS) $(OPT_FLAGS_O3) $(DEFINES)
CFLAGS_O2 = $(MOREFLAGS) $(CODE_FLAGS) $(OPT_FLAGS_O2) $(DEFINES)
LDFLAGS += $(MOREFLAGS)
ZSTD_FILES = zstd-1.4.5/lib/common/zstd_common.o
ZSTD_FILES += zstd-1.4.5/lib/common/fse_decompress.o
ZSTD_FILES += zstd-1.4.5/lib/common/xxhash.o
ZSTD_FILES += zstd-1.4.5/lib/common/error_private.o
ZSTD_FILES += zstd-1.4.5/lib/common/entropy_common.o
ZSTD_FILES += zstd-1.4.5/lib/common/pool.o
ZSTD_FILES += zstd-1.4.5/lib/common/debug.o
ZSTD_FILES += zstd-1.4.5/lib/common/threading.o
ZSTD_FILES += zstd-1.4.5/lib/compress/zstd_compress.o
ZSTD_FILES += zstd-1.4.5/lib/compress/zstd_compress_literals.o
ZSTD_FILES += zstd-1.4.5/lib/compress/zstd_compress_sequences.o
ZSTD_FILES += zstd-1.4.5/lib/compress/zstd_compress_superblock.o
ZSTD_FILES += zstd-1.4.5/lib/compress/zstdmt_compress.o
ZSTD_FILES += zstd-1.4.5/lib/compress/zstd_double_fast.o
ZSTD_FILES += zstd-1.4.5/lib/compress/zstd_fast.o
ZSTD_FILES += zstd-1.4.5/lib/compress/zstd_lazy.o
ZSTD_FILES += zstd-1.4.5/lib/compress/zstd_ldm.o
ZSTD_FILES += zstd-1.4.5/lib/compress/zstd_opt.o
ZSTD_FILES += zstd-1.4.5/lib/compress/fse_compress.o
ZSTD_FILES += zstd-1.4.5/lib/compress/huf_compress.o
ZSTD_FILES += zstd-1.4.5/lib/compress/hist.o
ZSTD_FILES += zstd-1.4.5/lib/decompress/zstd_decompress.o
ZSTD_FILES += zstd-1.4.5/lib/decompress/huf_decompress.o
ZSTD_FILES += zstd-1.4.5/lib/decompress/zstd_ddict.o
ZSTD_FILES += zstd-1.4.5/lib/decompress/zstd_decompress_block.o
ZSTD_FILES += zstd-1.4.5/lib/dictBuilder/divsufsort.o

all: clean GLZ_PART libglz.so
MKDIR = mkdir -p
GLZ_PART: $(ZSTD_FILES)
.c.o:
	@$(MKDIR) $(dir $@)
	$(CC) $(CFLAGS) $< -std=c99 -c -o $@

libglz.so: GLZ_PART
	$(CXX) -o libglz.so $(ZSTD_FILES) -shared -fPIC -L. -Wl,--whole-archive glz_arm.a libsecurec_arm.a -Wl,--no-whole-archive

clean:
	rm -rf *.o *.so zstd-1.4.5/lib/common/*.o zstd-1.4.5/lib/compress/*.o zstd-1.4.5/lib/decompress/*.o zstd-1.4.5/lib/dicBuilder/*.o
