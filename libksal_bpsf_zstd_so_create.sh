#!/bin/bash
set -e

execsuccess=0
execfailed=1

# release/debug
version_type=$1

if [ "$version_type" = "debug" ]; then
    echo "[ --- debug build in progress ---]"
else
    echo "[ --- release build in progress ---]"
fi

packagename="ksal_bpsf"
packagever=$(cat ./ksal_bpsf.spec | grep 'Version:' | head -1 | awk '{print $2}')

curd=$(pwd)
echo "Current dir: $curd"

static_library=./usr/lib64/libksal_bpsf.a
static_library_debug=./usr/lib64/libksal_bpsf_debug.a
include_file=./usr/include/ksal/ksal_bpsf.h
include_log_file=./usr/include/ksal/bpsf_log.h

if [ "$version_type" = "debug" ]; then
    rpm2cpio libksal-release-1.11.0.oe1.aarch64.rpm | cpio -idv $static_library_debug
    rpm2cpio libksal-release-1.11.0.oe1.aarch64.rpm | cpio -idv $include_file
    rpm2cpio libksal-release-1.11.0.oe1.aarch64.rpm | cpio -idv $include_log_file
    cp $static_library_debug ./
    cp $include_file ./
    cp $include_log_file ./
else
    rpm2cpio libksal-release-1.11.0.oe1.aarch64.rpm | cpio -idv $static_library
    rpm2cpio libksal-release-1.11.0.oe1.aarch64.rpm | cpio -idv $include_file
    rpm2cpio libksal-release-1.11.0.oe1.aarch64.rpm | cpio -idv $include_log_file
    cp $static_library ./
    cp $include_file ./
    cp $include_log_file ./
fi

tar -zxf "zstd-1.5.6.tar.gz"
cp ksal-bpsf-zstd.patch zstd-1.5.6/
cd zstd-1.5.6
patch -p1 < ksal-bpsf-zstd.patch
cd ..

if [ "$version_type" = "debug" ]; then
    make BUILD_TYPE=debug
else
    make
fi

rm -rf ./zstd-1.5.6
rm -rf ./usr
rm -rf libksal_bpsf.a
rm -rf libksal_bpsf_debug.a

function initialize()
{
    if [[ -d "$curd/rpmbuild" ]]; then
        rm -rf $curd/rpmbuild
    fi  
    mkdir -p $curd/rpmbuild/{BUILD,RPMS,SOURCES,SPECS,SRPMS,BUILDROOT}

    mkdir -p $curd/ksal_bpsf
    cp -rf $curd/ksal_bpsf.h $curd/ksal_bpsf/
    cp -rf $curd/bpsf_log.h $curd/ksal_bpsf/
    cp -rf $curd/libksal_bpsf.so $curd/ksal_bpsf/

    cp -rf $curd/ksal_bpsf/* $curd/rpmbuild/BUILD/
    cp $curd/ksal_bpsf.spec $curd/rpmbuild/SPECS/

    rm -rf $curd/ksal_bpsf/
    rm -rf $curd/libksal_bpsf.so
    rm -rf $curd/ksal_bpsf.h
    rm -rf $curd/bpsf_log.h
}

function pack_binary()
{
    local rpm_name="ksal_bpsf"
    cd "$curd/rpmbuild/SPECS" || { echo "enter SPECS dir failed"; exit $execfailed; }

    if [ "$version_type" = "debug" ]; then
        rpmbuild -bb --define "version_suffix debug" "ksal_bpsf.spec"
        rpm_name="ksal_bpsf_debug"
    else
        rpmbuild -bb "ksal_bpsf.spec"
    fi

    if [ $? -ne $execsuccess ]; then
        echo "rpmbuild failed"
        exit $execfailed
    fi

    if find "$curd/rpmbuild/RPMS/aarch64/" -name "$rpm_name-$packagever"*.aarch64.rpm | grep -q .; then
        echo "rpmbuild success"
    else
        echo "RPM not found"
        exit $execfailed
    fi
}

initialize
pack_binary