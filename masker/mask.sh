#!/bin/bash
cd masker
cd build

mkdir maskedpics

for e in *.jpg; do
	echo $e
	./masker $e ${e%.*}_mask.pbm ${e%.*}_new.jpg
	mv ${e%.*}_new.jpg maskedpics/${e%.*}_new.jpg
done 
