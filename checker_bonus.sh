#!/bin/bash

for i in {0..9};
do
        echo TESTUL $i Bonus----------------------------------------------------
        cp ./tests/bonus$i.in test.in
      valgrind --leak-check=full  ./movies -b test.in test.out
        diff -q test.out "./tests/ref/bonus/bonus"$i"_v0.ref"
done

