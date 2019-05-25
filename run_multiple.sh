#!/bin/sh

SUM=0
EXECUTABLE=$1

for threads in 1 2 3 4 5 6 7 8 9 10 11 12 13
do
    SUM=0
    for i in {1..10}
    do
        #TEMP=`./heated_plate_openmp`
        #echo $TEMP
        TIME_TAKEN=`export OMP_NUM_THREADS=$threads; ./$EXECUTABLE --dataset=./datasets/custom_11_d.txt --cities=11 | grep -i wallclock | cut -d = -f2 | cut -d ' ' -f2`
        #echo "TIME_TAKEN=$TIME_TAKEN"
        SUM=$(echo "$TIME_TAKEN + $SUM" | bc)
    done

    #echo "sum=$SUM"
    AVERAGE=$(echo "scale = 6; $SUM / 10" | bc)
    echo "$threads:AVERAGE=$AVERAGE"
    echo "========================"
done
