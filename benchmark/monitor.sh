#!/bin/sh

# Try to run gcc with version flag to check if it exists
gcc --version > /dev/null 2>&1
if [ $? -eq 0 ]; then
    echo "GCC found. Proceeding with compilation..."
    gcc -o dummy dummy_program.c
    echo "Compilation complete."
fi


MAX=4
while true; do
    echo "Insert how many processes you want to create:"
    read N_INPUT

    # Use expr to test if input is a valid integer
    if expr "$N_INPUT" + 0 >/dev/null 2>&1; then
        if [ "$N_INPUT" -le "$MAX" ]; then
            break
        else
            echo "The inserted number is too large (the maximum accepted value is $MAX)"
        fi
    else
        echo "Invalid input. Please enter a number."
    fi
done

echo "Launching the program $N_INPUT times..."

COUNT=1
while [ "$COUNT" -le "$N_INPUT" ]; do
    ./dummy &
    PID=$!
    PIDS="$PIDS $PID"
    echo "Program $COUNT launched (PID $PID)."
    COUNT=`expr "$COUNT" + 1`
done

echo "Monitoring the programs with PIDs $PIDS"

INTERVAL=0.5  # sampling interval in seconds
CLK_TCK=100 # THIS VALUE NEEDS TO BE VERIFIED


# Record process start times
for PID in $PIDS; do
    if [ ! -d "/proc/$PID" ]; then
        echo "PID $PID does not exist, skipping."
        continue
    fi

    OUTFILE="sched_monitor_$PID.csv"
    echo "Data for process with PID $PID" > "sched_monitor_$PID.csv"
    echo "" > "sched_monitor_$PID.csv"

    # Extract field 22 (starttime) from stat
    STARTTIME=`awk '{print $22}' /proc/$PID/stat`
    mkdir -p ./starttimes
    echo "$STARTTIME" > "./starttimes/starttime_$PID"

done

while :; do
    PIDS_LEFT=0

    for PID in $PIDS; do
        if [ ! -d "/proc/$PID" ]; then
        continue
        fi

        echo "Monitoring process $PID..."

        PIDS_LEFT=`expr $PIDS_LEFT + 1`

        STAT=`cat /proc/$PID/stat`
        UTIME=`echo "$STAT" | awk '{print $14}'`
        STIME=`echo "$STAT" | awk '{print $15}'`
        STARTTIME=`cat ./starttimes/starttime_$PID`

        USER_SEC=`echo "scale=2; $UTIME / $CLK_TCK" | bc`
        SYS_SEC=`echo "scale=2; $STIME / $CLK_TCK" | bc`
        CPU_TIME=`echo "scale=2; $USER_SEC + $SYS_SEC" | bc`

        UPTIME=`cut -d' ' -f1 /proc/uptime`
        START_SEC=`echo "scale=2; $STARTTIME / $CLK_TCK" | bc`
        ELAPSED_TIME=`echo "scale=2; $UPTIME - $START_SEC" | bc`
        WAIT_TIME=`echo "scale=2; $ELAPSED_TIME - $CPU_TIME" | bc`
        
        VOL_CS=`grep voluntary_ctxt_switches /proc/$PID/status | head -1 | awk '{print $3}'`
        INVOL_CS=`grep nonvoluntary_ctxt_switches /proc/$PID/status | head -1 | awk '{print $2}'`
        TIMESTAMP=`date +%s.%N 2>/dev/null || date +%s`

        {
        echo "timestamp: $TIMESTAMP"
        echo "user_time: $USER_SEC [s]"
        echo "system_time: $SYS_SEC [s]"
        echo "cpu_time: $CPU_TIME [s]"
        echo "elapsed_time: $ELAPSED_TIME [s]"
        echo "wait_time: $WAIT_TIME [s]"
        echo "voluntary_cs: $VOL_CS"
        echo "nonvoluntary_cs: $INVOL_CS"
        echo "---------------------------"   # Blank line to separate entries
        } >> "sched_monitor_$PID.csv"
        
    done

    if [ "$PIDS_LEFT" -eq 0 ]; then
        break
    fi
    # sleep $INTERVAL
done

rm -rf ./starttimes
echo "Monitoring complete."