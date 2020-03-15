#!/bin/bash
dirs=`find ./HamielecKarol -name cw* | tail -1`
lastdir=`basename $dirs`
number=${lastdir: -2}
number=$(($number + 1))
if [ $number -gt 9 ]; then
        newdircw="cw$number"
else
        newdircw="cw0$number"
fi

echo $newdircw
mkdir ./HamielecKarol/$newdircw

for i in `seq 1 $1`
do
        newdir="zad$i"
        echo $newdir
        mkdir ./HamielecKarol/$newdircw/$newdir
done
