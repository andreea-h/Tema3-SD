#!/bin/bash


for i in {0..14};
do
	echo TESTUL $i cerinta1----------------------------------------------------
	cp ./tests/test$i.in test.in
	./movies -c1 test.in test.out
	diff -s test.out "./tests/ref/task1/test"$i"_task1.ref"
done

for i in {0..14};
do
	  echo TESTUL $i cerinta2---------------------------------------------------
	  cp ./tests/test"$i"_task2.in test.in
       	./movies -c2 test.in test.out
      	diff -s test.out "./tests/ref/task2/test"$i"_task2.ref"
done

for i in {0..24};
do
        echo TESTUL $i cerinta3----------------------------------------------------
        cp ./tests/test$i.in test.in
	./movies -c3 test.in test.out

	diff -s test.out "./tests/ref/task3/test"$i"_task3.ref"
done

for i in {0..9};
do
	echo TEST BONUS $i---------------------------------------------------------
	cp ./tests/bonus$i.in test.in
	./movies -b test.in test.out
	diff -q test.out "./tests/ref/bonus/bonus"$i"_v0.ref"
done
