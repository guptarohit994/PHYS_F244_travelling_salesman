#!/bin/sh

SUM=0
EXECUTABLE=$1
CITIES=$2

if [ -z "$EXECUTABLE" ]
then
    echo "Error! Need an EXECUTABLE: depthfirst_parallel | depthfirst_serial"
    exit 1
else
    if [ ! -f "$EXECUTABLE" ]
    then 
        echo "Error! EXECUTABLE does not found!"
        exit 1
    fi
fi

if [ -z "$CITIES" ]
then
    echo "Error! CITIES not provided. Using 15 city dataset by default"
    CITIES=15
fi

DATASET_PATH="./datasets/lau15_dist_15c_d.txt"

if [ $CITIES -eq 5 ]
then
    DATASET_PATH="./datasets/five_d.txt"
elif [ $CITIES -eq 11 ]
then
    DATASET_PATH="./datasets/custom_11_d.txt"
elif [ $CITIES -eq 15 ]
then
    DATASET_PATH="./datasets/lau15_dist_15c_d.txt"
fi


echo "Running dataset with ${CITIES} cities and dataset is at ${DATASET_PATH}"

echo -e "\nthreads\t       \tAverage time (in seconds)"
echo "========================================="
for threads in 1 2 3 4 5 6 7 8 9 10 11 12 13
do
    SUM=0
    for i in {1..10}
    do
        #TEMP=`./heated_plate_openmp`
        #echo $TEMP
        TIME_TAKEN=`export OMP_NUM_THREADS=$threads; ./$EXECUTABLE --dataset="$DATASET_PATH" --cities="$CITIES" | grep -i wallclock | cut -d = -f2 | cut -d ' ' -f2`
        #echo "TIME_TAKEN=$TIME_TAKEN"
        SUM=$(echo "$TIME_TAKEN + $SUM" | bc)
    done

    #echo "sum=$SUM"
    AVERAGE=$(echo "scale = 6; $SUM / 10" | bc)
    echo -e "$threads\t       \t$AVERAGE"
    echo "========================================="
done
