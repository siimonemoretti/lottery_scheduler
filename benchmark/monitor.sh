#!/bin/sh

INTERVAL=0.1  # Seconds between polls

# Try to run gcc with version flag to check if it exists
gcc --version > /dev/null 2>&1
if [ $? -eq 0 ]; then
    echo "GCC found. Proceeding with compilation..."
    gcc -o dummy dummy_program.c
    echo "Compilation complete."
fi

echo "Launching the program..."
./dummy &
echo "Program launched."
PID=$!
echo "Its PID is $PID"
OUTFILE="sched_stats_$PID.log"

if [ ! -d "/proc/$PID" ]; then
  echo "Process $PID does not exist."
  exit 1
fi

echo "Timestamp,Exec Runtime (ns),Wait Sum (ns),Vol Ctx Switches,Nonvol Ctx Switches" > "$OUTFILE"
echo "Sarting the monitoring..."

while [ -d "/proc/$PID" ]; do
  TS=$(date +%s.%N)
  # Capture key lines from sched file
  SCHED_LINE=$(grep "^se.exec_start\|^se.sum_exec_runtime\|^se.wait_sum\|^nr_switches" /proc/$PID/sched)
  VOL_CTX=$(grep "^voluntary_ctxt_switches" /proc/$PID/status | awk '{print $2}')
  INVOL_CTX=$(grep "^nonvoluntary_ctxt_switches" /proc/$PID/status | awk '{print $2}')

  EXEC_RUNTIME=$(echo "$SCHED_LINE" | grep "sum_exec_runtime" | awk '{print $3}')
  WAIT_SUM=$(echo "$SCHED_LINE" | grep "se.wait_sum" | awk '{print $3}')
  NR_SWITCHES=$(echo "$SCHED_LINE" | grep "nr_switches" | awk '{print $3}')

  echo "$TS,$EXEC_RUNTIME,$WAIT_SUM,$VOL_CTX,$INVOL_CTX" >> "$OUTFILE"

  sleep $INTERVAL
done

echo "Monitoring stopped. Data saved to $OUTFILE"
