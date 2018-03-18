#!/bin/sh
set -e
if [ -d "temp" ]; then
    rm -rf temp
fi
mkdir temp 
cd temp
tar -xzf ../sqlite-snapshot-201802271540.tar.gz
cd sqlite-snapshot-201802271540
./configure --enable-shared=no --enable-static=yes --prefix=$1 > ./$2 2>&1
make install > ./$2 2>&1
cd ../.. 
rm -rf temp
