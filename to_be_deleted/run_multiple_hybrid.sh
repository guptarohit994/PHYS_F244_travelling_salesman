#!/bin/sh

SUM=0
EXECUTABLE=$1
CITIES=$2

PWD=`pwd`
echo "Currently in $PWD"
echo -e "Using slurm job id ${SLURM_JOB_ID}\n"

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

OUTFILE_PATH="${EXECUTABLE}_${SLURM_JOB_ID}.txt"
declare -a AVERAGE

echo "Running dataset with ${CITIES} cities and dataset is at ${DATASET_PATH}"

MAX_NTASKS=$(echo "$CITIES - 1" | bc)
echo -e "\nntasks:cpus-per-task\t       \tAv time (in seconds)    Speedup over serial (times)"
echo "==================================================================="

ntasks=1
while [ $MAX_NTASKS -ge $ntasks ]
do
    SUM=0
    CPUS=$(echo "a=$MAX_NTASKS; b=$ntasks; if ( a%b ) a/b+1 else a/b" | bc)
    for i in {1..2}
    do
        printf "ntasks:%d\n" "$ntasks"
        printf "cpus:%d\n" "$CPUS"
        #TEMP=`./heated_plate_openmp`
        #echo $TEMP
        TIME_TAKEN=`export OMP_NUM_THREADS=$CPUS; export MV2_ENABLE_AFFINITY=0; srun --pty --ntasks=$ntasks --cpus-per-task=$CPUS --nodes=$ntasks -p compute -t 00:30:00 --wait 0 mpirun ./$EXECUTABLE --dataset="$DATASET_PATH" --cities="$CITIES" --outfile="$OUTFILE_PATH" | grep -i wallclock | cut -d = -f2 | cut -d ' ' -f2`
        echo "TIME_TAKEN=$TIME_TAKEN"
        SUM=$(echo "$TIME_TAKEN + $SUM" | bc)
    done

    #echo "sum=$SUM"
    AVERAGE[$ntasks]=$(echo "scale = 6; $SUM / 10" | bc)
    SPEEDUP=$(echo "scale = 2; ${AVERAGE[1]} / ${AVERAGE[$ntasks]}" | bc)
    printf "%d:%d\t    \t%.6f\t  \t%.2f\n" "$ntasks" "$CPUS" "${AVERAGE[$ntasks]}" "$SPEEDUP"
    #echo -e "$threads\t       \t{$AVERAGE[$threads]}\t      \t$SPEEDUP"
    echo "==================================================================="
    true $(( ntasks++ ))
done

