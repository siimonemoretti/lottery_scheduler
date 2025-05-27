# Group 9 - Enhancing and Experimenting with the Linux Scheduler

- [Group 9 - Enhancing and Experimenting with the Linux Scheduler](#group-9---enhancing-and-experimenting-with-the-linux-scheduler)
	- [Modifications to the Linux Scheduler](#modifications-to-the-linux-scheduler)
		- [1. Implementing a New Scheduling Algorithm](#1-implementing-a-new-scheduling-algorithm)
			- [1. Define a new scheduling class](#1-define-a-new-scheduling-class)
			- [2. Tweak the linker file](#2-tweak-the-linker-file)
			- [3. Create the scheduling policy](#3-create-the-scheduling-policy)
			- [4. Initialize the data structures](#4-initialize-the-data-structures)
			- [5. Modify the task\_struct](#5-modify-the-task_struct)
			- [6. Implement the scheduling functions](#6-implement-the-scheduling-functions)
			- [7. Modify the Makefile](#7-modify-the-makefile)
		- [2. \[CHALLENGE\] Per-Core adaptive scheduling policy](#2-challenge-per-core-adaptive-scheduling-policy)
	- [Benchmarking the Scheduler](#benchmarking-the-scheduler)
	- [Performance Metrics](#performance-metrics)


## Modifications to the Linux Scheduler

### 1. Implementing a New Scheduling Algorithm

We decided to implement a new scheduling algorithm based on the lottery scheduling policy. Its behaviour is simple: when a process is created, it is assigned a number of tickets, which are used to determine the process's chance of being scheduled. The scheduler randomly picks a ticket from the available tickets and runs the process that holds that ticket. Of course, the more tickets a process has, the higher its chance of being scheduled.

To implement a new scheduling algorithm we had to modify the Linux kernel source code. The main steps were:

#### 1. Define a new scheduling class

Inside the folder 'linux/kernel/sched' we created a new file called 'lottery.c'. In this file, we defined a new scheduling class called 'lottery' via the DEFINE_SCHED_CLASS macro, like this:

```c
DEFINE_SCHED_CLASS(lottery) = {
	.enqueue_task = enqueue_task_lottery,
	.dequeue_task = dequeue_task_lottery,
	// And so on ...
};
```

This macro is used to define a new scheduling class and its associated functions. Basically the macro is useful to register the new scheduling class of type 'struct sched_class' in the kernel.

#### 2. Tweak the linker file

We then had to modify the linker file 'linux/include/asm-generic/vmlinux.lds.h' in order to include our new scheduling class. This is necessary because the scheduler will traverse the various scheduling classes not via an ordinary linked list (so via pointer), but simply by traversing the contiguous memory space where the scheduling classes are stored, knowing where the first one and the last one are located. To do this, we added the following line to the linker file:

```c
/*
 * The order of the sched class addresses are important, as they are
 * used to determine the order of the priority of each sched class in
 * relation to each other.
 */
#define SCHED_DATA				\
	STRUCT_ALIGN();				\
	__begin_sched_classes = .;		\
	*(__idle_sched_class)			\
	*(__fair_sched_class)			\
	*(__rt_sched_class)			\
	*(__dl_sched_class)			\
	*(__stop_sched_class)			\
	*(__lottery_sched_class)		\
	__end_sched_classes = .;
```

Also, we put the class at the end of the list, so that it has the lowest priority compared to the other scheduling classes.

#### 3. Create the scheduling policy

In order to create the a scheduling policy that the system would recognize, we had to modify the file 'linux/include/uapi/linux/sched.h'. In this file, we added a new scheduling policy called 'SCHED_LOTTERY' after the existing scheduling policies, like so:

```c
/*
 * Scheduling policies
 */
#define SCHED_NORMAL		0
#define SCHED_FIFO		1
#define SCHED_RR		2
#define SCHED_BATCH		3
/* SCHED_ISO: reserved but not implemented yet */
#define SCHED_IDLE		5
#define SCHED_DEADLINE		6
/* New scheduling policy for lottery scheduling */
#define SCHED_LOTTERY		7
```

#### 4. Initialize the data structures

Inside 'linux/kernel/sched/core.c' we modified the function *sched_init* that is called during the kernel initialization to initialize the runqueue for our new scheduling class. We created the function *init_ltr_rq* that we defined in 'lottery.c'.

The lottery runqueue is a data structure that keeps track of the tasks that are currently in the lottery scheduling class. It keeps track of:

- The total number of tickets available in the runqueue.
- The number of tasks in the runqueue.
- A doubly linked list of tasks in the runqueue. This kind of data structure was implemented to allow for efficient insertion and deletion of tasks in the runqueue by the linux kernel developer, also in order to avoid having too many implementations of the same data structure in the kernel.

#### 5. Modify the task_struct

The task_struct is the data structure that represents a task in the Linux kernel. We had to modify this kind of data structure to include in it a field of type *sched_ltr_entity*, a section that would hold the number of tickets acquired by a process, and in order to do so we modified the file 'linux/include/linux/sched.h'.

#### 6. Implement the scheduling functions

Then it was time to implement the scheduling functions for our new scheduling class. 

The main function that make up a scheduling class are:

- enqueue_task_lottery: This function is called when a task is added to the run queue. It adds the task to the lottery queue and updates the total number of tickets.
- dequeue_task_lottery: This function is called when a task is removed from the run queue. It removes the task from the lottery queue and updates the total number of tickets.
- yield_task_lottery: This function is called when a task voluntarily yields the CPU. It updates the task's state and may trigger a reschedule based on the lottery tickets.
- check_preempt_curr_lottery: This function is called to check if the current task should be preempted by a higher priority task. It compares the lottery tickets of the current task and the new task and decides whether to preempt or not.
- pick_next_task_lottery: This function is called to select the next task to run. It randomly selects a ticket from the available tickets and returns the task associated with that ticket.
- set_next_task_lottery: This function is called to set the next task to run. It updates the current task pointer to the selected task.
- task_tick_lottery: This function is called when a task is running and its time slice expires. It checks if the task should be rescheduled based on the lottery tickets and updates the task's state accordingly.

#### 7. Modify the Makefile

Finally, we had to modify the Makefile in the 'linux/kernel/sched' directory to include our new scheduling class. We added the following line:

```makefile
obj-y += lottery.o
```

### 2. [CHALLENGE] Per-Core adaptive scheduling policy

## Benchmarking the Scheduler



## Performance Metrics