import matplotlib.pyplot as plt
import matplotlib.patches as mpatches
from collections import defaultdict

filename = "third.log"

events = []
pid_info = {}  # To store the 'number' (tickets) for each PID

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

# For Gantt chart: list of (pid, start_time, duration)
gantt_data = []

for i, (pid, current_time) in enumerate(events):
    # Determine end of time slice
    if i + 1 < len(events):
        next_time = events[i + 1][1]
    else:
        next_time = current_time + 1  # Assume 1ns for last slice

    duration = next_time - current_time
    gantt_data.append((pid, current_time, duration))

    # Calculate waiting time
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

# Plot Gantt Chart with one horizontal bar per PID and colored segments
plt.figure(figsize=(12, 6))

# Assign a unique color per PID
unique_pids = sorted(set(pid for pid, _, _ in gantt_data))
pid_colors = {pid: plt.cm.tab20(i % 20) for i, pid in enumerate(unique_pids)}  # Up to 20 unique colors

y_map = {pid: idx for idx, pid in enumerate(unique_pids)}

for pid, start, duration in gantt_data:
    plt.barh(
        y=y_map[pid],
        width=duration,
        left=start,
        height=0.5,
        color=pid_colors[pid],
        edgecolor='black'
    )

# Set Y-axis labels
plt.yticks(
    range(len(unique_pids)),
    [f"PID {pid} ({pid_info.get(pid, '?')} tickets)" for pid in unique_pids]
)

# Build legend manually to avoid duplicates
legend_handles = []
for pid in unique_pids:
    label = f"PID {pid} ({pid_info.get(pid, '?')} tickets)"
    patch = mpatches.Patch(color=pid_colors[pid], label=label)
    legend_handles.append(patch)

plt.legend(handles=legend_handles, bbox_to_anchor=(1.05, 1), loc='upper left', title="Process Info")
plt.xlabel("Time (normalized) [ns]")
plt.title("Gantt Chart of Process Scheduling")
plt.grid(True, axis='x')
plt.tight_layout()
plt.savefig("gantt_4p_flex.png")
plt.show()
