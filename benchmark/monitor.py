import matplotlib.pyplot as plt
from collections import defaultdict

filename = "first.log"

events = []

with open(filename, 'r') as file:
    next(file)  # skip header

    first_time = None
    father_pid = None

    for line in file:
        line = line.strip()
        if not line or ',' not in line:
            continue
        parts = line.split(',')
        if len(parts) != 3:
            continue
        
        pid, time_str, _ = parts
        pid = pid.strip()
        time_val = float(time_str.strip())

        if father_pid is None:
            father_pid = pid
        
        if pid == father_pid:
            continue

        if first_time is None:
            first_time = time_val
        
        normalized_time = time_val - first_time
        events.append((pid, normalized_time))

events.sort(key=lambda x: x[1])

pid_times = defaultdict(list)
pid_waiting_times = defaultdict(list)          # Non-cumulative
pid_cumulative_waiting_times = defaultdict(list)  # Cumulative

last_scheduled_time = defaultdict(lambda: 0)
cumulative_waiting_time = defaultdict(lambda: 0)

for pid, current_time in events:
    last_time = last_scheduled_time[pid]
    waiting_time = current_time - last_time

    # Save both versions
    pid_waiting_times[pid].append(waiting_time)
    cumulative_waiting_time[pid] += waiting_time
    pid_cumulative_waiting_times[pid].append(cumulative_waiting_time[pid])

    pid_times[pid].append(current_time)
    last_scheduled_time[pid] = current_time

# Non-cumulative waiting time

plt.figure(figsize=(10, 6))
for pid in pid_times:
    times = pid_times[pid]
    waitings = pid_waiting_times[pid]
    plt.plot(times, waitings, marker='o', label=f"PID {pid}")

plt.xlabel("Time (normalized)")
plt.ylabel("Waiting Time")
plt.title("Waiting Times per PID over Time (Non-Cumulative)")
plt.legend()
plt.grid(True)
plt.tight_layout()
plt.savefig("waiting_times_plot_non_cumulative.png")

# Cumulative waiting time

plt.figure(figsize=(10, 6))
for pid in pid_times:
    times = pid_times[pid]
    waitings = pid_cumulative_waiting_times[pid]
    plt.plot(times, waitings, marker='o', label=f"PID {pid}")

plt.xlabel("Time (normalized)")
plt.ylabel("Cumulative Waiting Time")
plt.title("Cumulative Waiting Times per PID over Time")
plt.legend()
plt.grid(True)
plt.tight_layout()
plt.savefig("waiting_times_plot_cumulative.png")

