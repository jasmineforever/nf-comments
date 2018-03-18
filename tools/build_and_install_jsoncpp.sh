#!/bin/sh
set -e
if [ -d "temp" ]; then
    rm -rf temp
fi
mkdir temp 
cd temp
tar -xzf ../jsoncpp-0.y.tar.gz
cd jsoncpp-0.y.z
mkdir build
cd build
cmake -DJSONCPP_WITH_TESTS=OFF -DJSONCPP_WITH_POST_BUILD_UNITTEST=OFF \
 -DBUILD_SHARED_LIBS=OFF -DBUILD_STATIC_LIBS=ON -DCMAKE_INSTALL_PREFIX=$1 .. > ./$2 2>&1
make > ./$2 2>&1
make install > ./$2 2>&1
cd ../../.. 
rm -rf temp
