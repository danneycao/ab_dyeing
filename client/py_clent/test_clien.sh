#!/bin/sh

i=0
while [ $i -lt 100 ]
do
    i=`expr $i + 1`;
    echo $i
    ./ab_dyeing_client.py&
    usleep 100
done
