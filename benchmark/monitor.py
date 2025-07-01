import matplotlib.pyplot as plt
from collections import defaultdict

filename = "third.log"

events = []
pid_info = {}  # To store the 'number' for each PID

with open(filename, 'r') as file:
    next(file)  # skip header

    first_time = None
    father_pid = None

    for line in file:
        line = line.strip()
        if not line or ',' not in line:
            continue

        parts = line.split(',')
        if len(parts) != 4:
            continue  # Expecting 4 fields: pid, time, letter, number

        pid, time_str, _, number_str = parts
        pid = pid.strip()
        time_val = float(time_str.strip())
        number_val = number_str.strip()

        if father_pid is None:
            father_pid = pid

        if pid == father_pid:
            continue

        if first_time is None:
            first_time = time_val

        normalized_time = time_val - first_time
        events.append((pid, normalized_time))

        # Store the number associated with each PID (if not already stored)
        if pid not in pid_info:
            pid_info[pid] = number_val

# Sort events by normalized time
events.sort(key=lambda x: x[1])

# Data structures
pid_times = defaultdict(list)
pid_waiting_times = defaultdict(list)
pid_cumulative_waiting_times = defaultdict(list)

last_scheduled_time = defaultdict(lambda: 0)
cumulative_waiting_time = defaultdict(lambda: 0)

# Calculate waiting times
for pid, current_time in events:
    last_time = last_scheduled_time[pid]
    waiting_time = current_time - last_time

    pid_waiting_times[pid].append(waiting_time)
    cumulative_waiting_time[pid] += waiting_time
    pid_cumulative_waiting_times[pid].append(cumulative_waiting_time[pid])

    pid_times[pid].append(current_time)
    last_scheduled_time[pid] = current_time

# Plot Non-Cumulative Waiting Time
plt.figure(figsize=(10, 6))
for pid in pid_times:
    times = pid_times[pid]
    waitings = pid_waiting_times[pid]
    label = f"PID {pid} ({pid_info.get(pid, '?')} tickets)"
    plt.plot(times, waitings, marker='o', label=label)

plt.xlabel("Time (normalized) [ns]")
plt.ylabel("Waiting Time [ns]")
plt.title("Waiting Times per PID over Time (Non-Cumulative)")
plt.legend()
plt.grid(True)
plt.tight_layout()
plt.savefig("wtnc_4p_flex.png")

# Plot Cumulative Waiting Time
plt.figure(figsize=(10, 6))
for pid in pid_times:
    times = pid_times[pid]
    waitings = pid_cumulative_waiting_times[pid]
    label = f"PID {pid} ({pid_info.get(pid, '?')} tickets)"
    plt.plot(times, waitings, marker='o', label=label)

plt.xlabel("Time (normalized) [ns]")
plt.ylabel("Cumulative Waiting Time [ns]")
plt.title("Cumulative Waiting Times per PID over Time")
plt.legend()
plt.grid(True)
plt.tight_layout()
plt.savefig("wtc_4p_flex.png")
