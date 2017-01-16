#!/bin/bash

for f in `ls *.thrift`
do
  thrift -r --gen py $f 
done

rm -rf gen-py/*.skeleton.cpp

exit 0
