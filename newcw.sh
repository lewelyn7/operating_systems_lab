#!/bin/bash
dirs=`find ./HamielecKarol -name cw* | tail -1`
lastdir=`basename $dirs`
number=${lastdir: -2}
if [ 9 > $number ]; then
        newdircw="cw0$(($number + 1))"
else
        newdircw="cw$(($number + 1))"
fi

echo $newdircw
mkdir ./HamielecKarol/$newdircw

for i in `seq 1 $1`
do
        newdir="zad$i"
        echo $newdir
        mkdir ./HamielecKarol/$newdircw/$newdir
done
