# 项目介绍

存储算法加速库（简称KSAL）是华为自研的存储算法加速库，当前包括EC算法、CRC16 T10DIF算法、CRC32C算法、memcpy优化算法、DAS智能预取算法和Ceph百亿对象存储元数据zstd压缩算法。 关于KSAL的详细特性介绍可参考[存储加速算法库](https://gitee.com/link?target=https%3A%2F%2Fwww.hikunpeng.com%2Fdocument%2Fdetail%2Fzh%2Fkunpengsdss%2FbasicAccelFeatures%2Fksal%2Fkunpengksal_16_0001.html)，关于zstd算法的详细特性介绍可参考[Zstandard - Real-time data compression algorithm \(facebook.github.io\)](https://gitee.com/link?target=https%3A%2F%2Ffacebook.github.io%2Fzstd%2F)

# 版本说明

<table><thead align="left"><tr id="row9183105817568"><th class="cellrowborder" valign="top" width="33.33333333333333%" id="mcps1.1.4.1.1"><p id="p71831584563"><a name="p71831584563"></a><a name="p71831584563"></a>Kunpeng zstd</p>
</th>
<th class="cellrowborder" valign="top" width="33.33333333333333%" id="mcps1.1.4.1.2"><p id="p3183758135614"><a name="p3183758135614"></a><a name="p3183758135614"></a>开源zstd</p>
</th>
<th class="cellrowborder" valign="top" width="33.33333333333333%" id="mcps1.1.4.1.3"><p id="p17183185835612"><a name="p17183185835612"></a><a name="p17183185835612"></a>特性</p>
</th>
</tr>
</thead>
<tbody><tr id="row10183105865617"><td class="cellrowborder" valign="top" width="33.33333333333333%" headers="mcps1.1.4.1.1 "><p id="p11183165885610"><a name="p11183165885610"></a><a name="p11183165885610"></a>v1.0.0</p>
</td>
<td class="cellrowborder" valign="top" width="33.33333333333333%" headers="mcps1.1.4.1.2 "><p id="p1418385885620"><a name="p1418385885620"></a><a name="p1418385885620"></a>1.5.2</p>
</td>
<td class="cellrowborder" valign="top" width="33.33333333333333%" headers="mcps1.1.4.1.3 "><p id="p20183958165617"><a name="p20183958165617"></a><a name="p20183958165617"></a>优化压缩性能</p>
</td>
</tr>
</tbody>
</table>

# 环境部署

鲲鹏优化后的zstd库仅支持运行在华为鲲鹏硬件平台上。

执行目录中的build.sh脚本即可编译生成相关动态库以及可执行程序，完成安装部署。

执行安装脚本后，会编译生成相关动态库以及可执行程序。

动态库位于当前源码目录的zstd-Kzstar\_1.5.2/lib子目录中：

```
lrwxrwxrwx. 1 root root      66 Sep 17 09:49 libzstd.so -> libzstd.so.1.5.2
lrwxrwxrwx. 1 root root      66 Sep 17 09:49 libzstd.so.1 -> libzstd.so.1.5.2
-rwxr-xr-x. 1 root root  908552 Sep 17 09:49 libzstd.so.1.5.2
```

可执行程序位于源码目录的zstd-Kzstar\_1.5.2子目录中：

```
lrwxrwxrwx.  1 root root     13 Sep 17 09:49 zstd
```

# 快速上手

安装生成的zstd二进制文件以及相关动态库的使用方法和开源zstd均保持一致。若鲲鹏优化版本zstd动态库位于/usr/local/kzstar/lib下，可以通过替换原有动态库或者设置环境变量LD\_LIBRARY\_PATH=/usr/local/kzstar/lib/:$LD\_LIBRARY\_PATH的方式使用鲲鹏优化版本的zstd动态库。

可以使用ldd命令查看上层软件程序所依赖的zstd动态库路径是否与鲲鹏优化版本zstd动态库保持一致

```
[root@localhost lzbench]# ldd lzbench      
linux-vdso.so.1 (0x0000ffff94bbd000)      
libz.so.1 => /usr/lib64/libz.so.1 (0x0000ffff94b4f000)      
libzstd.so.1 => /usr/local/kzstar/lib/libzstd.so.1 (0x0000ffff85c89000)      
liblz4.so.1 => /home/kplz4/lib/liblz4.so.1 (0x0000ffff94a0d000)      
libstdc++.so.6 => /usr/lib64/libstdc++.so.6 (0x0000ffff94817000)      
libm.so.6 => /usr/lib64/libm.so.6 (0x0000ffff94776000)      
libgcc_s.so.1 => /usr/lib64/libgcc_s.so.1 (0x0000ffff94745000)      
libc.so.6 => /usr/lib64/libc.so.6 (0x0000ffff94596000)      
/lib/ld-linux-aarch64.so.1 (0x0000ffff94b80000)
```

# 安装后验证

```
[root@localhost kzstar]# ./zstd -V  
*** zstd command line interface 64-bits v1.5.2, by Yann Collet ***  
*** This version is optimized by Kunpeng, based on the open source zstd. ***
```

# 贡献指南

如果使用过程中有任何问题，或者需要反馈特性需求和bug报告，可以提交isssues联系我们，具体贡献方法可参考[这里](https://gitcode.com/boostkit/community/blob/master/docs/contributor/contributing.md)。

# 免责声明

此代码仓计划参与zstd软件开源，仅作性能提升，编码风格遵照原生开源软件，继承原生开源软件安全设计，不破坏原生开源软件设计及编码风格和方式，软件的任何漏洞与安全问题，均由相应的上游社区根据其漏洞和安全响应机制解决。请密切关注上游社区发布的通知和版本更新。鲲鹏计算社区对软件的漏洞及安全问题不承担任何责任。

