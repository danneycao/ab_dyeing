#!/bin/bash

bin=../bin/abclient

if [ $# != 0 ] 
then
  make -j4
  if [ $? != 0 ]
  then
    echo "build error"
    exit -1
  fi
fi

args="./config.conf \
"

#gdb --ex run --args \
  $bin $args
