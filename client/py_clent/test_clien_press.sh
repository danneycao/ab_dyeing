#!/bin/sh

i=0
while [ $i -lt 10 ]
do
    i=`expr $i + 1`;
    echo $i
    ./test_clien.sh&
    usleep 100
done
