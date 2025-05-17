#!/bin/sh

INTERVAL=1
DURATION=10
TOTAL_TIME=0
ACTIVE_TIME=0

echo "Compiling the program..."
gcc -o dummy dummy_program.c

echo "Program compiled."
./dummy &
PID1=$!
./dummy &
PID2=$!
./dummy &
PID3=$!
./dummy &
PID4=$!

# if [ -z "$PID" ] || [ -z "$DURATION" ]; then
#   echo "Usage: $0 <pid> <duration in seconds>"
#   exit 1
# fi

echo "Monitoring process $PID1, $PID2, $PID3, $PID4 for $DURATION seconds..."
echo "Time,CPU%,State,Wait_Time" > wait_time_monitor.csv

# Get initial CPU time
INITIAL_UTIME1=$(grep "^utime" /proc/$PID1/stat 2>/dev/null | awk '{print $1}' || echo 0)
INITIAL_STIME1=$(grep "^stime" /proc/$PID1/stat 2>/dev/null | awk '{print $1}' || echo 0)
INITIAL_UTIME2=$(grep "^utime" /proc/$PID2/stat 2>/dev/null | awk '{print $1}' || echo 0)
INITIAL_STIME2=$(grep "^stime" /proc/$PID2/stat 2>/dev/null | awk '{print $1}' || echo 0)
INITIAL_UTIME3=$(grep "^utime" /proc/$PID3/stat 2>/dev/null | awk '{print $1}' || echo 0)
INITIAL_STIME3=$(grep "^stime" /proc/$PID3/stat 2>/dev/null | awk '{print $1}' || echo 0)
INITIAL_UTIME4=$(grep "^utime" /proc/$PID4/stat 2>/dev/null | awk '{print $1}' || echo 0)
INITIAL_STIME4=$(grep "^stime" /proc/$PID4/stat 2>/dev/null | awk '{print $1}' || echo 0)

i=1

while [ $i -le $DURATION ]; do
    # Get CPU usage percentage
    CPU_USAGE1=$(ps -p $PID1 -o %cpu= 2>/dev/null | tr -d ' ' || echo "0")
    CPU_USAGE2=$(ps -p $PID2 -o %cpu= 2>/dev/null | tr -d ' ' || echo "0")
    CPU_USAGE3=$(ps -p $PID3 -o %cpu= 2>/dev/null | tr -d ' ' || echo "0")
    CPU_USAGE4=$(ps -p $PID4 -o %cpu= 2>/dev/null | tr -d ' ' || echo "0")

    # Get process state (R=running, S=sleeping, D=disk sleep, etc.)
    STATE1=$(ps -p $PID1 -o state= 2>/dev/null | tr -d ' ' || echo "?")
    STATE2=$(ps -p $PID2 -o state= 2>/dev/null | tr -d ' ' || echo "?")
    STATE3=$(ps -p $PID3 -o state= 2>/dev/null | tr -d ' ' || echo "?")
    STATE4=$(ps -p $PID4 -o state= 2>/dev/null | tr -d ' ' || echo "?") 

    # Increment counters
    TOTAL_TIME1=$((TOTAL_TIME1 + INTERVAL))
    if [ "$STATE1" = "R" ]; then
    ACTIVE_TIME1=$((ACTIVE_TIME1 + INTERVAL))
    fi
    TOTAL_TIME2=$((TOTAL_TIME2 + INTERVAL))
    if [ "$STATE2" = "R" ]; then
    ACTIVE_TIME2=$((ACTIVE_TIME2 + INTERVAL))
    fi
    TOTAL_TIME3=$((TOTAL_TIME3 + INTERVAL))
    if [ "$STATE3" = "R" ]; then
    ACTIVE_TIME3=$((ACTIVE_TIME3 + INTERVAL))
    fi
    TOTAL_TIME4=$((TOTAL_TIME4 + INTERVAL))
    if [ "$STATE4" = "R" ]; then
    ACTIVE_TIME4=$((ACTIVE_TIME4 + INTERVAL))
    fi
    # Calculate wait time so far
    WAIT_TIME1=$((TOTAL_TIME1 - ACTIVE_TIME1))
    # echo "Wait time 1: $WAIT_TIME1 = $TOTAL_TIME1 - $ACTIVE_TIME1"
    WAIT_TIME2=$((TOTAL_TIME2 - ACTIVE_TIME2))
    WAIT_TIME3=$((TOTAL_TIME3 - ACTIVE_TIME3))
    WAIT_TIME4=$((TOTAL_TIME4 - ACTIVE_TIME4))

    # echo "$i,$CPU_USAGE1,$STATE1,$WAIT_TIME1" >> wait_time_$PID1.csv
    echo "Second $i: CPU=$CPU_USAGE1%, State=$STATE1, Wait time so far=${WAIT_TIME1}s"
    # echo "$i,$CPU_USAGE2,$STATE2,$WAIT_TIME2" >> wait_time_$PID2.csv
    echo "Second $i: CPU=$CPU_USAGE2%, State=$STATE2, Wait time so far=${WAIT_TIME2}s"
    # echo "$i,$CPU_USAGE3,$STATE3,$WAIT_TIME3" >> wait_time_$PID3.csv
    echo "Second $i: CPU=$CPU_USAGE3%, State=$STATE3, Wait time so far=${WAIT_TIME3}s"
    # echo "$i,$CPU_USAGE4,$STATE4,$WAIT_TIME4" >> wait_time_$PID4.csv
    echo "Second $i: CPU=$CPU_USAGE4%, State=$STATE4, Wait time so far=${WAIT_TIME4}s"

    sleep $INTERVAL
    i=$((i + 1))
done

echo "Monitoring of $PID1 complete. Estimated wait time: ${WAIT_TIME1}s out of ${TOTAL_TIME1}s total."
echo "Monitoring of $PID2 complete. Estimated wait time: ${WAIT_TIME2}s out of ${TOTAL_TIME2}s total."
echo "Monitoring of $PID3 complete. Estimated wait time: ${WAIT_TIME3}s out of ${TOTAL_TIME3}s total."
echo "Monitoring of $PID4 complete. Estimated wait time: ${WAIT_TIME4}s out of ${TOTAL_TIME4}s total."
# echo "Results saved to wait_time_$PID.csv"
