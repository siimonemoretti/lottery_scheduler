#!/bin/sh

INTERVAL=0.1  # Seconds between polls

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

echo "Starting the monitoring..."
while :; do
  PIDS_LEFT=0
  for PID in $PIDS; do
    while [ -d "/proc/$PID" ]; do
      TS=$(date +%s.%N)
      # Capture key lines from sched file
      SCHED_LINE=$(grep "^se.exec_start\|^se.sum_exec_runtime\|^se.wait_sum\|^nr_switches" /proc/$PID/sched)
      VOL_CTX=$(grep "^voluntary_ctxt_switches" /proc/$PID/status | awk '{print $2}')
      INVOL_CTX=$(grep "^nonvoluntary_ctxt_switches" /proc/$PID/status | awk '{print $2}')

      EXEC_RUNTIME=$(echo "$SCHED_LINE" | grep "sum_exec_runtime" | awk '{print $3}')
      WAIT_SUM=$(echo "$SCHED_LINE" | grep "se.wait_sum" | awk '{print $3}')
      NR_SWITCHES=$(echo "$SCHED_LINE" | grep "nr_switches" | awk '{print $3}')
      {
        echo "Starting time: $TS" 
        echo "Execution runtime: $EXEC_RUNTIME"
        echo "Waiting time: $WAIT_SUM"
        echo "Voluntary context switching: $VOL_CTX"
        echo "Non voluntary context switching: $INVOL_CTX"
        exho "" # Empty line
      } >> "$OUTFILE"
      sleep $INTERVAL
    done
    if [ "$PIDS_LEFT" -eq 0 ]; then
      break
    fi

  sleep $INTERVAL
  done
done
echo "Monitoring stopped. Data saved to $OUTFILE"
