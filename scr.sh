#!/bin/bash

mkdir -p spam/

for i in {1..100}; do
    echo $i;
    ./gen > spam/${i}.in;
    ./1 < spam/${i}.in > spam/${i}.out
    ./hirschberg < spam/${i}.in > spam/${i}.ans
    diff -Z spam/${i}.out spam/${i}.ans
done
