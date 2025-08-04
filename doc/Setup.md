# BPSF算法使用指南
## 支持CPU
华为鲲鹏处理器

## 支持软件版本
zstd 1.5.6

## 支持操作系统
openEuler

## 软件包下载链接
[BPSF算法下载地址](https://gitee.com/kunpengcompute/zstd/releases/tag/ksal_bpsf)

# 一、软件下载与环境准备
## 1. 安装rpmbuild工具
（1）	创建路径并进入该路径
```
mkdir -p /home/ksal_bpsf
cd /home/ ksal_bpsf
```
（2）	获取BoostKit-KSAL_1.11.0.zip，放置于`/home/ksal_bpsf`目录下
（3）	在“/home/ksal_bpsf”目录下面解压BoostKit-KSAL_1.11.0.zip。
```
unzip BoostKit-KSAL_1.11.0.zip
```
（4）	获取zstd-1.5.6.tar.gz，放置于`/home/ksal_bpsf`目录下。
（5）	获取编译所需文件，包括：Makefile、ksal-bpsf-zstd.patch、ksal_bpsf.spec和libksal_bpsf_zstd_so_create.sh，并将下载的文件放置于`/home/ksal_bpsf`目录下。
（6）	安装rpmbuild。
```
yum install rpmdevtools -y
rpmdev-setuptree
```
## 2. 修改rpmbuild构建目录
（7）	将“rpmbuild”目录更改至`/home/ksal_bpsf`目录下
执行rpmbuild安装命令之后，修改“.rpmmacros”文件。将“%_topdir”地址修改为`/home/ksal_bpsf/rpmbuild`。
```
vi /root/.rpmmacros
```
修改完后，再次执行rpmbuild安装命令。
```
rpmdev-setuptree
```

# 二、制作RPM包
## 制作RPM包（release版本）
（1） 在`/home/ksal_bpsf`目录下执行如下命令，生成用于KSAL BPSF安装部署RPM包。
```
cd /home/ksal_bpsf/
sh libksal_bpsf_zstd_so_create.sh
```
（2）	安装生成的RPM包。
```
cd /home/ksal_bpsf/rpmbuild/RPMS/aarch64
rpm -ivh ksal_bpsf-1.0.0-openEuler.aarch64.rpm
```
（3）	执行如下命令查看RPM安装情况。
```
rpm -qi ksal_bpsf-1.0.0-openEuler.aarch64
```
（4）	确认安装路径。
执行以下命令查看`/usr/lib64/`和`/usr/include`目录下的文件列表，确认KSAL BPSF动态库文件与KSAL BPSF头文件是否都位于此目录下。
```
ll /usr/lib64/libksal_bpsf.so
ll /usr/include/ksal_bpsf.h
```
（5）	使用时，应链接动态库。
```
-lksal_bpsf
```
（6）	使用时，应配置环境变量。
```
export LD_LIBRARY_PATH=/usr/lib64:$LD_LIBRARY_PATH
```
（7）	软件包卸载
```
yum remove ksal_bpsf -y
```

## 制作RPM包（debug版本）
（1）	`/home/ksal_bpsf`目录下执行如下命令，生成用于KSAL BPSF安装部署RPM包。
```
cd /home/ksal_bpsf/
sh libksal_bpsf_zstd_so_create.sh debug
```
（2）	安装生成的RPM包。
```
cd /home/ksal_bpsf/rpmbuild/RPMS/aarch64
rpm -ivh ksal_bpsf_debug-1.0.0-openEuler.aarch64.rpm
```
（3）	使用时，应配置环境变量。
```
export LD_LIBRARY_PATH=/usr/lib64:$LD_LIBRARY_PATH
```
（4）	使用时，链接动态库。
```
-lksal_bpsf
```
（5）	软件包卸载
```
yum remove ksal_bpsf_debug -y
```


# 四、使用实例
（1）	打开文件`test_bpsf.c`
```
vi test_bpsf.c
```
输入以下内容：
```
#include "ksal_bpsf.h"
#include <stdio.h>
#include <stdlib.h>

const size_t BLOCK_SIZE_4096 = 4096;
const size_t BLOCK_SIZE_4160 = 4160;
const size_t SEGMENT_COUNT = 8;
const uint8_t kValue = 0x10;

int main() {
    const size_t src_len = BLOCK_SIZE_4096 * SEGMENT_COUNT;
    uint8_t p_src[src_len];
    size_t dst_len = BPSF_compressBound(src_len);
    uint8_t p_dst[dst_len];
    uint16_t offset[8] = {0};
    uint16_t len[8] = {0};

    for (size_t i = 0; i < src_len; i++) {
        p_src[i] = i * kValue;
    }

    int compress_result = BPSF_compress(p_src, src_len, BLOCK_SIZE_4096, p_dst, &dst_len, offset, len);
    printf("compress result: %d\n", compress_result);

    int start = 0;
    int end = 3;
    uint8_t decompressed_data[BLOCK_SIZE_4096 * (end - start + 1)];
    size_t decompressed_len = 0;

    uint16_t union_offset, union_len;
    int union_result = BPSF_union(offset, len, start, end, &union_offset, &union_len);
    printf("union result: %d\n", union_result);

    for (int i = 0; i < 8; i++) {
        int decompress_result = BPSF_decompress(
            p_dst + union_offset,
            union_len,
            end - start + 1,
            BLOCK_SIZE_4096,
            decompressed_data,
            &decompressed_len
        );
        printf("decompress result: %d\n", decompress_result);
    }
    return 0;
}

```
输入`:wq!`退出

（2）	使用示例
```
gcc test_bpsf.c -o test_bpsf -lksal_bpsf
./test_bpsf
```
输出result均为0即为成功

# 五、日志
（1）	日志说明
通过查看`/var/log/messages`，即可查看bpsf输出内容
Release包输出日志等级为Error级别，Debug包输出Error与Info级别日志
其中，日志头文件在`/usr/include/bpsf_log.h`
（2）	日志使用
用户可查看BPSF算法日志部分头文件，将日志文件对接到BPSF中
（3）新建文件`test_bpsf_log.c`

```
vi test_bpsf_log.c
```
输入以下内容：
```

#include "ksal_bpsf.h"
#include "bpsf_log.h"
#include <stdio.h>
#include <stdlib.h>

const size_t BLOCK_SIZE_4096 = 4096;
const size_t BLOCK_SIZE_4160 = 4160;
const size_t SEGMENT_COUNT = 8;
const uint8_t kValue = 0x10;

static void customer_logger(LogLevel level, const char *message) {
    const char *level_str;
    switch (level) {
        case LOG_ERR:
            level_str = "ERROR";
            break;
        case LOG_INFO:
            level_str = "INFO";
            break;
        default:
            level_str = "ERROR";
    }
    printf("[%s] %s\n", level_str, message);
}

int main() {
    SetLogFunction(customer_logger); // 传入自定义日志函数customer_logger

    const size_t src_len = BLOCK_SIZE_4096 * SEGMENT_COUNT;
    uint8_t p_src[src_len];
    size_t dst_len = BPSF_compressBound(src_len);
    uint8_t p_dst[dst_len];
    uint16_t offset[8] = {0};
    uint16_t len[8] = {0};

    for (size_t i = 0; i < src_len; i++) {
        p_src[i] = i * kValue;
    }

    int compress_result = BPSF_compress(p_src, src_len, BLOCK_SIZE_4096, p_dst, &dst_len, offset, len);
    printf("compress result: %d\n", compress_result);

    int start = 0;
    int end = 3;
    uint8_t decompressed_data[BLOCK_SIZE_4096 * (end - start + 1)];
    size_t decompressed_len = 0;

    uint16_t union_offset, union_len;
    int union_result = BPSF_union(offset, len, start, end, &union_offset, &union_len);
    printf("union result: %d\n", union_result);

    for (int i = 0; i < 8; i++) {
        int decompress_result = BPSF_decompress(
            p_dst + union_offset,
            union_len,
            end - start + 1,
            BLOCK_SIZE_4096,
            decompressed_data,
            &decompressed_len
        );
        printf("decompress result: %d\n", decompress_result);
    }
    return 0;
}
```
（3）	使用示例
```
gcc test_bpsf_log.c -o test_bpsf_log -lksal_bpsf
./test_bpsf_log
```
使用SetLogFunction(customer_logger)时，可在自定义日志里出现BPSF日志，而在不设置时，默认出现在`/var/log/messages`里
