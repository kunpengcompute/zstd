# KSAL ZSTD
## 介绍
存储算法加速库（简称KSAL）是华为自研的存储算法加速库，当前包括EC算法、CRC16 T10DIF算法、CRC32C算法、memcpy优化算法、DAS智能预取算法和Ceph百亿对象存储元数据zstd压缩算法。
关于KSAL的详细特性介绍可参考[存储加速算法库](https://www.hikunpeng.com/document/detail/zh/kunpengsdss/basicAccelFeatures/ksal/kunpengksal_16_0001.html)，关于zstd算法的详细特性介绍可参考[Zstandard - Real-time data compression algorithm (facebook.github.io)](https://facebook.github.io/zstd/)

## 仓库说明
本仓库主要用于KSAL zstd算法包的编译与安装

## 支持CPU
华为鲲鹏920处理器

## 支持软件版本
Ceph 14.2.8
zstd 1.5.6

## 支持操作系统
openEuler 20.03 LTS SP1
openEuler 22.03 LTS SP1


## 使用说明
详细使用说明可参考以下文档：
[# 编译安装KSAL zstd算法包](https://www.hikunpeng.com/document/detail/zh/kunpengsdss/basicAccelFeatures/ksal/kunpengksal_16_0025.html)


## 参与贡献
如果您想为本仓库贡献代码，请向本仓库任意maintainer发送邮件；
如果您找到产品中的任何Bug，欢迎您提出ISSUE