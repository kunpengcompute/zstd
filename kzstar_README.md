## 安装

1. 使用如下命令获取源码：

   ```shell
   git clone https://gitee.com/kunpengcompute/zstd.git -b Kzstar_1.5.2
   ```

2. 获取包含KZstar头文件及动态库的压缩包，并拷贝至zstd目录下

3. 使用如下命令进行编译安装：

   ```shell
   sh kzstar_build.sh install
   ```

4. 检查安装情况：

   ```shell
   tree /usr/local/kzstar
   ```

   若回显结果如下所示，则说明安装成功：

   ```shell
   [root@localhost zstd]# tree /usr/local/kzstar/
   /usr/local/kzstar/
   ├── include
   │   ├── zstar.h
   │   └── zstd.h
   └── lib
       ├── libsecurec.so
       ├── libzstar.so
       ├── libzstd.so -> libzstd.so.1.5.2
       ├── libzstd.so.1 -> libzstd.so.1.5.2
       └── libzstd.so.1.5.2
   ```

## 支持接口说明

当前KZSTAR仅支持块压缩解压算法，并且只支持level1-4等级的压缩，其他等级会按levle5进行处理。涉及接口如下所示：

```
/**
 * 该函数根据输入数据的大小计算压缩后所需的最大输出缓冲区大小。
 *
 * @param[in] srcSize 输入数据的大小（字节数）
 * 
 * @return 返回压缩后的最大字节数，即为压缩所需的缓冲区大小。
 */
size_t ZSTD_compressBound(size_t srcSize)；

/**
 * 该函数用于获取已经压缩的数据帧的原始大小。
 * 
 * @param[in] src 数据源，压缩后的数据流
 * @param[in] srcSize 输入数据的大小（字节数）
 * 
 * @return 返回压缩数据帧的内容大小（字节数）。
 */
unsigned long long ZSTD_getFrameContentSize(const void *src, size_t srcSize);

/**
 * 该函数创建并初始化一个压缩上下文。压缩上下文用于控制压缩过程的各个参数。
 * 
 * @return 返回一个指向压缩上下文的指针。
 */
ZSTD_CCtx* ZSTD_createCCtx(void);

/**
 * 该函数释放由 `ZSTD_createCCtx` 创建的压缩上下文，释放相关资源。
 * 
 * @param[in] cctx 要释放的压缩上下文指针。
 * 
 * @return 返回 `0` 表示成功。如果失败，返回一个负数错误码。
 */
size_t ZSTD_freeCCtx(ZSTD_CCtx* cctx);

/**
 * 该函数使用给定的压缩上下文 `cctx` 对数据进行压缩。可以控制压缩等级。
 * 
 * @param[in] cctx 压缩上下文，包含压缩相关设置。
 * @param[out] dst 输出缓冲区，存储压缩后的数据。
 * @param[in] dstCapacity 输出缓冲区的容量（字节数）。
 * @param[in] src 输入数据，指向要压缩的数据。
 * @param[in] srcSize 输入数据的大小（字节数）。
 * @param[in] compressionLevel 压缩等级。
 * 
 * @return 返回实际压缩的数据大小。
 */
size_t ZSTD_compressCCtx(ZSTD_CCtx* cctx, void* dst, size_t dstCapacity,  const void* src, size_t srcSize, int compressionLevel);

/**
 * 该函数使用默认的压缩设置对数据进行压缩。
 * 
 * @param[out] dst 输出缓冲区，存储压缩后的数据。
 * @param[in] dstCapacity 输出缓冲区的容量（字节数）。
 * @param[in] src 输入数据，指向要压缩的数据。
 * @param[in] srcSize 输入数据的大小（字节数）。
 * @param[in] compressionLevel 压缩等级。
 * 
 * @return 返回实际压缩的数据大小。
 */
size_t ZSTD_compress(void* dst, size_t dstCapacity, const void* src, size_t srcSize, int compressionLevel);

/** 
 * 该函数创建一个解压缩上下文，并允许传入自定义的内存分配和释放函数。
 * 
 * @param[in] customMem 自定义内存分配函数。
 * 
 * @return 返回一个指向解压缩上下文的指针。
 */
ZSTD_DCtx* ZSTD_createDCtx_advanced(ZSTD_customMem customMem);

/** 
 * 该函数创建并初始化一个解压缩上下文，使用默认的内存分配方式。
 * 
 * @return 返回一个指向解压缩上下文的指针。
 */
ZSTD_DCtx* ZSTD_createDCtx(void);

/**
 * 该函数释放由 `ZSTD_createDCtx` 或 `ZSTD_createDCtx_advanced` 创建的解压缩上下文。
 * 
 * @param[in] dctx 解压缩上下文指针。
 * 
 * @return 返回 `0` 表示成功。如果失败，返回负值。
 */
size_t ZSTD_freeDCtx(ZSTD_DCtx* dctx);

/**
 * 该函数使用一个已创建的解压缩上下文 (`dctx`) 对压缩数据进行解压缩。
 * 
 * @param[in] dctx 解压缩上下文。
 * @param[out] dst 解压缩后的数据存储缓冲区。
 * @param[in] dstCapacity `dst` 缓冲区的大小（字节数）。
 * @param[in] src 输入压缩数据缓冲区。
 * @param[in] srcSize 输入压缩数据的大小（字节数）。
 * 
 * @return 返回解压缩后的字节数。如果发生错误，返回负值。
 */
size_t ZSTD_decompressDCtx(ZSTD_DCtx* dctx, void* dst, size_t dstCapacity, const void* src, size_t srcSize);

/** 
 * 该函数使用默认的解压缩设置对压缩数据进行解压缩。
 * 
 * @param[out] dst 解压缩后的数据存储缓冲区。
 * @param[in] dstCapacity `dst` 缓冲区的大小（字节数）。
 * @param[in] src 输入压缩数据缓冲区。
 * @param[in] srcSize 输入压缩数据的大小（字节数）。
 * 
 * @return 返回解压缩后的字节数。如果发生错误，返回负值。
 */
size_t ZSTD_decompress(void* dst, size_t dstCapacity, const void* src, size_t srcSize);
```

## 使用案例

#### CTX接口调用用例

```c
char* zstd_init()
{
    zstd_params_s* zstd_params = (zstd_params_s*) malloc(sizeof(zstd_params_s));
    if (!zstd_params) return NULL;
    zstd_params->cctx = ZSTD_createCCtx();
    zstd_params->dctx = ZSTD_createDCtx();
    zstd_params->cdict = NULL;
    return (char*) zstd_params;
}

int64_t zstd_compress(char *inbuf, size_t insize, char *outbuf, size_t outsize, size_t level, char* workmem)
{
    size_t res;

    zstd_params_s* zstd_params = (zstd_params_s*) workmem;
    if (!zstd_params || !zstd_params->cctx) return 0;

    zstd_params->zparams = ZSTD_getParams(level, insize, 0);
    ZSTD_CCtx_setParameter(zstd_params->cctx, ZSTD_c_compressionLevel, level);
    zstd_params->zparams.fParams.contentSizeFlag = 1;
    
    res = ZSTD_compressCCtx(zstd_params->cctx, outbuf, outsize, inbuf, insize, level);
    if (ZSTD_isError(res)) return res;

    return res;
}

int64_t zstd_decompress(char *inbuf, size_t insize, char *outbuf, size_t outsize, char* workmem)
{
    zstd_params_s* zstd_params = (zstd_params_s*) workmem;
    if (!zstd_params || !zstd_params->dctx) return 0;

    return ZSTD_decompressDCtx(zstd_params->dctx, outbuf, outsize, inbuf, insize);
}

void zstd_deinit(char* workmem)
{
    zstd_params_s* zstd_params = (zstd_params_s*) workmem;
    if (!zstd_params) return;
    if (zstd_params->cctx) ZSTD_freeCCtx(zstd_params->cctx);
    if (zstd_params->dctx) ZSTD_freeDCtx(zstd_params->dctx);
    if (zstd_params->cdict) ZSTD_freeCDict(zstd_params->cdict);
    free(workmem);
}
```

#### 简单接口调用用例

```c
// 1. 输入数据
const char *input_data = "This is the input data that we want to compress using ZSTAR!";
size_t insize = strlen(input_data) + 1;

// 2. 分配内存
size_t compress_bound = ZSTD_compressBound(insize);
void *outbuf = malloc(compress_bound);

// 3. 压缩数据
size_t compressed_size = ZSTD_compress(outbuf, compress_bound, input_data, insize, 3);  // 使用压缩级别 3

// 4. 解压数据
size_t dst_size = ZSTD_getFrameContentSize(outbuf, compressed_size); // 获取解压后的数据大小
void *decompressed_data = malloc(dst_size);
size_t decompressed_size = ZSTD_decompress(decompressed_data, dst_size, outbuf, compressed_size);
```

#### 使能KZstar

根据KZstar的安装位置，配置对应环境变量，使程序正确链接KZstar库

```shell
[root@localhost zstdTestCode]# export LD_LIBRARY_PATH=/usr/local/kzstar/lib:$LD_LIBRARY_PATH
[root@localhost zstdTestCode]# ldd ZstdSimpleCompress
	linux-vdso.so.1 (0x0000ffff83646000)
	libzstd.so.1 => /usr/local/kzstar/lib/libzstd.so.1 (0x0000ffff83528000)
	libc.so.6 => /usr/lib64/libc.so.6 (0x0000ffff83379000)
	libzstar.so => /usr/local/kzstar/lib/libzstar.so (0x0000ffff83348000)
	/lib/ld-linux-aarch64.so.1 (0x0000ffff83609000)
	libsecurec.so => /usr/local/kzstar/lib/libsecurec.so (0x0000ffff83317000)
```

## 并发压缩指南

当存在空闲CPU资源时，可以通过配置对应的环境变量，实现压缩包的拆分，将压缩解压并行化，从而在不修改业务代码的前提下提升解压缩性能。

* **ZSTAR_THREAD_NUM_ENV**

> 1. 该环境变量用于设置可使用的线程数量，该线程数量包含了初始的主线程，即创建的子线程数量实际为ZSTAR_THREAD_NUM_ENV减1。
> 2. 线程数量默认为0，最大为17。
> 3. 当不设置线程数量，或者线程数量被设置为0或者1时，表示不启用多线程。
> 4. 当线程数量设置超过17时，则会将线程数量设置为17。
> 5. 使用非流式并行化解压时，必须要使用非流式并行化压缩生成的数据，解压时实际使用到的线程数量为压缩和解压使用到的线程数量的较小值。
> 6. 使用非流式压缩或解压时，主线程会参与到压缩或解压过程，实际线程数量与设置的线程数量一致；使用流式压缩时，主线程不参与压缩过程，实际线程数量为设置的线程数量减1

* **ZSTAR_THREAD_COMPRESS_LIMIT_ENV**

> 1. 该环境变量用于设置非流式压缩时，开启并行化所需数据量的最低大小。
> 2. 设置的单位为字节（Byte）。
> 3. 默认值为512KB，输入合规的正整数可以对并行化下限进行调整，其余如负数、非整形数等会被判断为无效值，仍使用默认下限。
> 4. 当非流式压缩输入数据大小大于等于设置的下限时，才会进行并行化压缩；否则使用单线程压缩。

注：说明：并发解压需要基于并发压缩的包才有效果，否则只按单任务方式解压。

## 性能测试

[kunpeng-lzbench](https://gitee.com/kunpeng_compute/lzbench)是基于lzbench的测试框架，并将zstd等算法修改为动态库的调用方式，用于对比不同压缩算法库之间的解压缩性能。下载源码，使用make方式编译生成lzbench二进制工具，基于itemdata文件测试解压缩性能。

* 调用原生zstd算法库测试块压缩性能，指定压缩等级为level 3，分块大小为128KB：

  ```shell
  [root@localhost lzbench]# ldd lzbench
  	linux-vdso.so.1 (0x0000ffffae181000)
  	libz.so.1 => /usr/lib64/libz.so.1 (0x0000ffffae113000)
  	libzstd.so.1 => /usr/lib64/libzstd.so.1 (0x0000ffffae012000)
  	liblz4.so.1 => /usr/lib64/liblz4.so.1 (0x0000ffffadfe1000)
  	libstdc++.so.6 => /usr/lib64/libstdc++.so.6 (0x0000ffffaddeb000)
  	libm.so.6 => /usr/lib64/libm.so.6 (0x0000ffffadd4a000)
  	libgcc_s.so.1 => /usr/lib64/libgcc_s.so.1 (0x0000ffffadd19000)
  	libc.so.6 => /usr/lib64/libc.so.6 (0x0000ffffadb6a000)
  	/lib/ld-linux-aarch64.so.1 (0x0000ffffae144000)
  [root@localhost lzbench]# ./lzbench -ezstd,3 -b128 itemdata 
  lzbench 1.8 (64-bit Linux)  (null)
  Assembled by P.Skibinski
  
  Compressor name         Compress. Decompress. Compr. size  Ratio Filename
  memcpy                  37655 MB/s 37041 MB/s     7316868 100.00 itemdata
  zstd 1.5.5 -3             195 MB/s   851 MB/s     2257863  30.86 itemdata
  done... (cIters=1 dIters=1 cTime=1.0 dTime=2.0 chunkSize=128KB cSpeed=0MB)
  
  ```

* 调用KZstar算法库测试块压缩性能，指定压缩等级为level 3，分块大小为128KB：

  ```shell
  [root@localhost lzbench]# export LD_LIBRARY_PATH=/usr/local/kzstar/lib:$LD_LIBRARY_PATH
  [root@localhost lzbench]# ldd lzbench
  	linux-vdso.so.1 (0x0000ffffa84cf000)
  	libz.so.1 => /usr/lib64/libz.so.1 (0x0000ffffa8461000)
  	libzstd.so.1 => /usr/local/kzstar/lib/libzstd.so.1 (0x0000ffffa8380000)
  	liblz4.so.1 => /usr/lib64/liblz4.so.1 (0x0000ffffa834f000)
  	libstdc++.so.6 => /usr/lib64/libstdc++.so.6 (0x0000ffffa8159000)
  	libm.so.6 => /usr/lib64/libm.so.6 (0x0000ffffa80b8000)
  	libgcc_s.so.1 => /usr/lib64/libgcc_s.so.1 (0x0000ffffa8087000)
  	libc.so.6 => /usr/lib64/libc.so.6 (0x0000ffffa7ed8000)
  	/lib/ld-linux-aarch64.so.1 (0x0000ffffa8492000)
  	libzstar.so => /usr/local/kzstar/lib/libzstar.so (0x0000ffffa7ea7000)
  	libsecurec.so => /usr/local/kzstar/lib/libsecurec.so (0x0000ffffa7e76000)
  [root@localhost lzbench]# ./lzbench -ezstd,3 -b128 itemdata 
  lzbench 1.8 (64-bit Linux)  (null)
  Assembled by P.Skibinski
  
  Compressor name         Compress. Decompress. Compr. size  Ratio Filename
  memcpy                  37578 MB/s 37182 MB/s     7316868 100.00 itemdata
  zstd 1.5.5 -3             261 MB/s  1030 MB/s     2266113  30.97 itemdata
  done... (cIters=1 dIters=1 cTime=1.0 dTime=2.0 chunkSize=128KB cSpeed=0MB)
  ```
  不配置并发优化选项，只使用普通优化手段，对比原生zstd算法，压缩性能和解压性能有一定提升。

* 调用KZstar算法库测试块压缩性能，设置环境变量**ZSTAR_THREAD_NUM_ENV=3**，指定压缩等级为level 3，分块大小为128KB与512KB：

  ```shell
  [root@localhost lzbench]# ZSTAR_THREAD_NUM_ENV=3 ./lzbench -ezstd,3 -b128 itemdata 
  lzbench 1.8 (64-bit Linux)  (null)
  Assembled by P.Skibinski
  
  Compressor name         Compress. Decompress. Compr. size  Ratio Filename
  memcpy                   6512 MB/s 37298 MB/s     7316868 100.00 itemdata
  zstd 1.5.5 -3             261 MB/s  1030 MB/s     2266113  30.97 itemdata
  done... (cIters=1 dIters=1 cTime=1.0 dTime=2.0 chunkSize=128KB cSpeed=0MB)
  [root@localhost lzbench]# ZSTAR_THREAD_NUM_ENV=3 ./lzbench -ezstd,3 -b512 itemdata 
  lzbench 1.8 (64-bit Linux)  (null)
  Assembled by P.Skibinski
  
  Compressor name         Compress. Decompress. Compr. size  Ratio Filename
  memcpy                   6447 MB/s 37817 MB/s     7316868 100.00 itemdata
  zstd 1.5.5 -3             588 MB/s  2084 MB/s     2221068  30.36 itemdata
  done... (cIters=1 dIters=1 cTime=1.0 dTime=2.0 chunkSize=512KB cSpeed=0MB)
  ```

  配置**ZSTAR_THREAD_NUM_ENV=3**，使用多线程进行并行压缩。对于128K包长，按单线程处理，无法使用并发能力；512K包长达到并发条件，压缩解压性能提升明显。

* 调用KZstar算法库测试块压缩性能，设置环境变量**ZSTAR_THREAD_NUM_ENV=3**以及**ZSTAR_THREAD_COMPRESS_LIMIT_ENV=1024**，指定压缩等级为level 3，分块大小为128KB与512KB：

  ```shell
  [root@localhost lzbench]# ZSTAR_THREAD_COMPRESS_LIMIT_ENV=1024 ZSTAR_THREAD_NUM_ENV=3 ./lzbench -ezstd,3 -b128 itemdata 
  lzbench 1.8 (64-bit Linux)  (null)
  Assembled by P.Skibinski
  
  Compressor name         Compress. Decompress. Compr. size  Ratio Filename
  memcpy                  38627 MB/s 36890 MB/s     7316868 100.00 itemdata
  zstd 1.5.5 -3             725 MB/s  1785 MB/s     2464904  33.69 itemdata
  done... (cIters=1 dIters=1 cTime=1.0 dTime=2.0 chunkSize=128KB cSpeed=0MB)
  [root@localhost lzbench]# ZSTAR_THREAD_COMPRESS_LIMIT_ENV=1024 ZSTAR_THREAD_NUM_ENV=3 ./lzbench -ezstd,3 -b512 itemdata 
  lzbench 1.8 (64-bit Linux)  (null)
  Assembled by P.Skibinski
  
  Compressor name         Compress. Decompress. Compr. size  Ratio Filename
  memcpy                  38242 MB/s 39856 MB/s     7316868 100.00 itemdata
  zstd 1.5.5 -3             664 MB/s  2271 MB/s     2232919  30.52 itemdata
  done... (cIters=1 dIters=1 cTime=1.0 dTime=2.0 chunkSize=512KB cSpeed=0MB)
  ```
  
  **ZSTAR_THREAD_COMPRESS_LIMIT_ENV=1024**表示非流式压缩时，开启并行化所需数据量的最低大小为1024B，此时-b128和-b512均能触发并发压缩。

### Note

1. KZstar当前仅支持块压缩部分接口，针对具体的业务使用场景，请自行检验可行性。
2. KZstar并发优化是基于拆分压缩包，进行并发处理从而提高性能；但是拆包方式可能会导致压缩比指标的劣化，并且劣化程度和数据集相关。因此，对于对压缩比要求较高或数据集不适合并发处理的业务场景，不建议开启并发功能。