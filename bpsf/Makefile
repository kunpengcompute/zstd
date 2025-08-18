CC = gcc

BUILD_TYPE ?= release
CFLAGS += -fPIC -fvisibility=hidden -O3

ifeq ($(BUILD_TYPE),debug)
	LIB_NAME = libksal_bpsf_debug.a
else
	LIB_NAME = libksal_bpsf.a
endif

ZSTD_SRC_DIR := zstd-1.5.6
BUILD_DIR := build

ZSTD_SRC_C := $(shell find $(ZSTD_SRC_DIR)/lib -name "*.c")

ZSTD_SRC_O = $(patsubst $(ZSTD_SRC_DIR)/lib/%.c,$(ZSTD_SRC_DIR)/lib/%.o,$(ZSTD_SRC_C))

all_objs = $(ZSTD_SRC_O)

all: clean KSAL_BPSF_PART libksal_bpsf.so
MKDIR = mkdir -p
KSAL_BPSF_PART: $(all_objs)
.c.o:
	@$(MKDIR) $(dir $@)
	$(CC) $(CFLAGS) $< -std=c99 -c -o $@

libksal_bpsf.so: KSAL_BPSF_PART
	$(AR) x $(LIB_NAME)
	$(CC) $(CFLAGS) -shared -o libksal_bpsf.so $(all_objs) *.o
	rm -rf *.o

clean:
	rm -rf *.o *.so zstd-1.5.6/lib/common/*.o zstd-1.5.6/lib/compress/*.o zstd-1.5.6/lib/decompress/*.o