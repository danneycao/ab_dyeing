#!/bin/bash

for f in `ls *.thrift`
do
  thrift -r --gen cpp $f 
done

rm -rf gen-cpp/*.skeleton.cpp

exit 0
