/*
 * Lottery scheduling class implementation (Mapped to SCHED_LOTTERY policy)
 */
#include "sched.h"
#include "pelt.h"
#include <linux///printk.h>
#include <linux/random.h>

/*
  * Log macro for logging op's (arrival time, finish time, etc.) in a CSV fashion.
  * - "e" for enqueue
  * - "d" for dequeue
  * - "s" for select
  * - "f" for finish
  */
#define log_event(pid, op, tickets)                                               \
	do {                                                                           \
		printk(KERN_DEFAULT ">>%d,%llu,%s,%d\n", pid, ktime_get_ns(), op, tickets); \
	} while (0)

void init_ltr_rq(struct ltr_rq *ltr_rq)
{
	// Initialize the lottery scheduling runqueue
	// Initialize the list head for the queue
	INIT_LIST_HEAD(&ltr_rq->queue);
	// Set the number of running tasks to 0
	ltr_rq->nr_running = 0;
	// Set the total number of tickets to 0
	ltr_rq->total_tickets = 0;
	// Print a message indicating that the lottery runqueue has been initialized
	//printk(KERN_EMERG ">> init_ltr_rq() Lottery runqueue initialized\n");
}

/*
  * Called when a task enters a runnable state. 
  * It puts the scheduling entity (task) into the lottery queue.
  */
static void enqueue_task_lottery(struct rq *rq, struct task_struct *p,
				 int flags)
{
	if (p->__state != TASK_RUNNING) {
		// Check if the task is waking up
		if (p->__state == TASK_WAKING) {
			//printk(KERN_ERR ">> enqueue_task_lottery() Task %s [%d] is WAKING up now\n", p->comm, p->pid);
			//dump_stack();
		} else {
			//printk(KERN_ERR ">> enqueue_task_lottery() Task %s [%d] is NOT in state 0, it's in state %ld\n", p->comm, p->pid, (long)p->__state);
			//dump_stack();
		}
	}

	// Enqueue the task into the lottery scheduling circular doubly linked list
	list_add_tail(&p->ltr.list, &rq->ltr.queue);

	// Increment the number of running tasks and total tickets in the runqueue
	rq->ltr.nr_running++;
	rq->ltr.total_tickets += p->ltr.tickets;
	// Check the task's state
	// Print the number of tickets the task has (+ PID) and the CPU it is on
	//printk(KERN_EMERG ">> enqueue_task_lottery() Task %s [%d] enqueued with %d tickets\n", p->comm, p->pid, p->ltr.tickets);

	// LOG that the task has been enqueued
	log_event(p->pid, "e", p->ltr.tickets);
}

/*
  * When a task is no longer runnable, it is removed from the lottery queue.
  * And it decrements the nr_running and total_tickets in the runqueue.
  */
static void dequeue_task_lottery(struct rq *rq, struct task_struct *p,
				 int flags)
{
	// Dequeue the task from the lottery scheduling queue
	list_del(&p->ltr.list);
	// Decrement the number of running tasks and total tickets in the runqueue
	rq->ltr.nr_running--;
	rq->ltr.total_tickets -= p->ltr.tickets;
	//printk(KERN_EMERG ">> dequeue_task_lottery() Task %s [%d] has been removed from the queue\n", p->comm, p->pid);

	// LOG that the task has been dequeued
	log_event(p->pid, "d", p->ltr.tickets);
}

/*
  * This function is called when a task yields the CPU voluntarily.
  */
static void yield_task_lottery(struct rq *rq)
{
	//printk(KERN_EMERG ">> Yielding task %s\n", current->comm);
	// TODO : Handle this
	// How do we implement the yield?
}

/*
  * This function checks if a task that entered the runnable state should preempt the currently running task.
  */
static void check_preempt_curr_lottery(struct rq *rq, struct task_struct *p,
				       int flags)
{
	//printk(KERN_EMERG ">> check_preempt_curr() called, triggering the scheduler\n");
	// We don't implement preemption in this scheduling class,
	// we just trigger the scheduler to pick the next task
	// based on the lottery ticket system.
}

/*
  * This function randomly picks the next task to run based on the lottery ticket system.
  * It's the task that should run NOW, not the NEXT task.
  */
static struct task_struct *pick_next_task_lottery(struct rq *rq)
{
	struct task_struct *task = NULL;
	struct list_head *pos;
	unsigned int total_tickets = rq->ltr.total_tickets;
	unsigned int lucky_one = 0;
	unsigned int tickets_cnt = 0;

	if (total_tickets == 0) {
		//printk(KERN_ERR ">> pick_next_task() No tasks in the lottery queue\n");
		return NULL;
	}

	// Check if there are any tasks in the queue
	if (list_empty(&rq->ltr.queue)) {
		//printk(KERN_ERR ">> pick_next_task() No tasks in the lottery queue\n");
		return NULL;
	}

	// Generate a random number between 0 and total_tickets
	get_random_bytes((void *)&lucky_one, sizeof(lucky_one));

	// Normalize the random number to be within the range of total_tickets
	lucky_one = lucky_one % total_tickets;

	// Iterate through the lottery queue to find the task with the lucky ticket
	// Iterating from the head (ltr.queue) using pos as iterator
	list_for_each (pos, &rq->ltr.queue) {
		task = list_entry(pos, struct task_struct, ltr.list);
		// Check its policy
		if (task->policy != SCHED_LOTTERY)
			continue;

		// Check if the lucky ticket is within the range of the current task's tickets
		if (lucky_one < (tickets_cnt + task->ltr.tickets) &&
		    lucky_one >= tickets_cnt) {
			break;
		}

		// Increment the ticket count
		tickets_cnt += task->ltr.tickets;
	}
	// task shouldn't be NULL here, but just in case
	if (task == NULL) {
		//printk(KERN_ERR ">> pick_next_task() No task found in the lottery queue\n");
		return NULL;
	}

	// Logging selected task
	log_event(task->pid, "s", task->ltr.tickets);

	//printk(KERN_EMERG ">> Task %s [PID %d] has been selected by the lottery (lucky ticket: %d)\n", task->comm, task->pid, lucky_one);
	// return the task that has been selected by the lottery
	return task;
}

/*
  * put_prev_task is called whenever a task is to be taken out of the CPU. 
  */
static void put_prev_task_lottery(struct rq *rq, struct task_struct *p)
{
	//printk(KERN_EMERG ">> put_prev_task_lottery() called for task %s\n", p->comm);
}

static void set_next_task_lottery(struct rq *rq, struct task_struct *p,
				  bool first)
{
	//printk(KERN_EMERG ">> set_next_task_lottery() called for task %s\n", p->comm);
}

/*
  * This function is mostly called from the time tick functions. Its what drives the preemption.
  * p is the task that is currently running.
  * rq is the runqueue that the task is on.
  * IDEA: we can randomly decide if we want to preempt the current task or not.
  */
static void task_tick_lottery(struct rq *rq, struct task_struct *p, int queued)
{
	// //printk(KERN_EMERG ">> task_tick_lottery() called for task %s\n", p->comm);
	unsigned int should_preempt = 0;

	if (rq->ltr.nr_running <= 1) {
		return;
	}

	get_random_bytes((void *)&should_preempt, sizeof(should_preempt));

	// If even, preempt the current task

	// Preempting on 1 / 10 cases
	if ((should_preempt % 10) == 0) {
		// resched the current task
		resched_curr(rq);
	} else {
	}
	return;
}

static void switched_to_lottery(struct rq *rq, struct task_struct *p)
{
	// Nothing to do yet
	// This is called in sched_setscheduler() for example
	//printk(KERN_EMERG ">> switched_to_lottery() called for task %s\n", p->comm);
}

static void switched_from_lottery(struct rq *rq, struct task_struct *p)
{
	// Nothing to do yet
	//printk(KERN_EMERG ">> switched_from_lottery() called for task %s\n", p->comm);

	if (!list_empty(&p->ltr.list)) {
		dequeue_task_lottery(rq, p, 0);
	}
}

static void prio_changed_lottery(struct rq *rq, struct task_struct *p,
				 int oldprio)
{
	// Nothing to do yet
	//printk(KERN_EMERG ">> prio_changed_lottery() called for task %s\n", p->comm);
}

DEFINE_SCHED_CLASS(lottery) = {
	.enqueue_task = enqueue_task_lottery,
	.dequeue_task = dequeue_task_lottery,
	.yield_task = yield_task_lottery,

	.check_preempt_curr = check_preempt_curr_lottery,

	.pick_next_task = pick_next_task_lottery,
	.put_prev_task = put_prev_task_lottery,
	.set_next_task = set_next_task_lottery,
	.task_tick = task_tick_lottery,

	.switched_to = switched_to_lottery,
	.switched_from = switched_from_lottery,
	.prio_changed = prio_changed_lottery,
};