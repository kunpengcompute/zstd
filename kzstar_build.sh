#!/bin/sh
set -e
SRC_PATH=$(pwd)

print_help()
{
    echo "--------------------------build kzstar Parameters:------------------------------------"
    echo "sh build.sh install                       |  编译安装zstd和kzstar"
    echo "sh build.sh clean                         |  清理环境"
    echo "--------------------------------------------------------------------------------------"
    return
}


clear_env()
{
    rm -rf /usr/local/kzstar/
    rm -rf ./zstar.h
    rm -rf ./libsecurec.so
    rm -rf ./libzstar.so
    make clean
}

unpackzstar(){
    print_help
    rm -rf /usr/local/kzstar/
    mkdir -p /usr/local/kzstar/lib
    mkdir -p /usr/local/kzstar/include

    tar -zxvf kzstar.tar.gz

    cp ./zstar.h /usr/local/kzstar/include
    cp ./libsecurec.so /usr/local/kzstar/lib
    cp ./libzstar.so /usr/local/kzstar/lib
}

buildzstd(){
    make -j64
    cp ./lib/libzstd.so.1.5.2 /usr/local/kzstar/lib
    cp ./lib/zstd.h /usr/local/kzstar/include
    cd /usr/local/kzstar/lib
    ln -s libzstd.so.1.5.2 libzstd.so
    ln -s libzstd.so.1.5.2 libzstd.so.1
    cd -
}

function main()
{

	if [ "$1" = "install" ];then
	    echo "build zstar"
        unpackzstar
        buildzstd
	elif [ "$1" = "help" ];then
            print_help
    elif [ "$1" = "clean" ];then
            echo "clean evp"
            clear_env
    else
	    print_help
	fi
}

main "$@"
exit $?
