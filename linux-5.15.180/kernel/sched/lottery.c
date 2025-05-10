/*
 * Lottery scheduling class implementation (Mapped to SCHED_LOTTERY policy)
 */
#include "sched.h"
#include "pelt.h"
#include <linux/printk.h>
#include <linux/random.h>

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
	printk(KERN_INFO "Lottery runqueue initialized\n");
}

/*
 * Called when a task enters a runnable state. 
 * It puts the scheduling entity (task) into the lottery queue.
 */
static void 
enqueue_task_lottery(struct rq *rq, struct task_struct *p, int flags)
{
	// Print with highest priority
	printk(KERN_EMERG ">>Trying to enqueue task %s\n", p->comm);
	// Enqueue the task into the lottery scheduling circular doubly linked list
	list_add_tail(&p->ltr.list, &rq->ltr.queue);

	// Increment the number of running tasks and total tickets in the runqueue
	rq->ltr.nr_running++;
   rq->ltr.total_tickets += p->ltr.tickets;
	printk(KERN_INFO "New task %s is added to the lottery queue (had %d default tickets)\n", p->comm, p->ltr.tickets);
}

/*
 * When a task is no longer runnable, it is removed from the lottery queue.
 * And it decrements the nr_running and total_tickets in the runqueue.
 */
static void
dequeue_task_lottery(struct rq *rq, struct task_struct *p,int flags)
{
	// Print with highest priority
	printk(KERN_EMERG ">>Trying to dequeue task %s\n", p->comm);
	// Dequeue the task from the lottery scheduling queue
   list_del(&p->ltr.list);
	// Decrement the number of running tasks and total tickets in the runqueue
	rq->ltr.nr_running--;
	rq->ltr.total_tickets -= p->ltr.tickets;
	printk(KERN_INFO "Task %s is removed from the lottery queue\n", p->comm);
}

/*
 * This function is called when a task yields the CPU voluntarily.
 */
static void yield_task_lottery(struct rq *rq)
{
	printk(KERN_EMERG ">>Yielding task %s\n", current->comm);
	// TODO : Handle this
	// For now: do nothing
}

/*
 * This function checks if a task that entered the runnable state should preempt the currently running task.
 */
static void check_preempt_curr_lottery(struct rq *rq, struct task_struct *p,
				       int flags)
{
	printk(KERN_EMERG ">>Checking if task %s should preempt the current task\n", p->comm);
}

/*
 * This function randomly picks the next task to run based on the lottery ticket system.
 * It's the task that should run NOW, not the NEXT task.
 */
static struct task_struct *pick_next_task_lottery(struct rq *rq)
{
	printk(KERN_EMERG ">>Picking the next task to run\n");
	struct task_struct *task = NULL;
	struct list_head *pos;
	unsigned int total_tickets = rq->ltr.total_tickets;
	unsigned int lucky_one = 0;
	unsigned int tickets_cnt = 0;

	if (total_tickets == 0) {
		return NULL;
	}

	// Generate a random number between 0 and total_tickets
	get_random_bytes((void *)&lucky_one, sizeof(lucky_one));

	// Normalize the random number to be within the range of total_tickets
	lucky_one = lucky_one % total_tickets;

	// Iterate through the lottery queue to find the task with the lucky ticket
	// Iterating from the head (ltr.queue) using pos as iterator
	list_for_each(pos, &rq->ltr.queue) {
		task = list_entry(pos, struct task_struct, ltr.list);
		// Check its policy
		if (task->policy != SCHED_LOTTERY)
			continue;

		// Check if the lucky ticket is within the range of the current task's tickets
		if (lucky_one < (tickets_cnt + task->ltr.tickets) && lucky_one >= tickets_cnt) {
			break;
		}

		// Increment the ticket count
		tickets_cnt += task->ltr.tickets;
	}
	// task shouldn't be NULL here, but just in case
	if (task == NULL) {
		printk(KERN_ERR "No task found in the lottery queue\n");
		return NULL;
	}

	// return the task that has been selected by the lottery
	return task;
}

static void put_prev_task_lottery(struct rq *rq, struct task_struct *p)
{
	printk(KERN_EMERG ">> put_prev_task_lottery() called for task %s\n", p->comm);
}

static void set_next_task_lottery(struct rq *rq, struct task_struct *p,
				  bool first)
{
	printk(KERN_EMERG ">> set_next_task_lottery() called for task %s\n", p->comm);
	
}

/*
 * This function is mostly called from the time tick functions. Its what drives the preemption.
 * p is the task that is currently running.
 * rq is the runqueue that the task is on.
 * IDEA: we can randomly decide if we want to preempt the current task or not.
 */
static void task_tick_lottery(struct rq *rq, struct task_struct *p, int queued)
{
	printk(KERN_EMERG ">> task_tick_lottery() called for task %s\n", p->comm);
	unsigned int should_preempt = 0;

	get_random_bytes((void *)&should_preempt, sizeof(should_preempt));
	 
	// If even, preempt the current task
	if (should_preempt && 1) // Preempt
		resched_curr(rq);
}

static void switched_to_lottery(struct rq *rq, struct task_struct *p)
{
	// Nothing to do yet
	printk(KERN_EMERG ">> switched_to_lottery() called for task %s\n", p->comm);
}

static void switched_from_lottery(struct rq *rq, struct task_struct *p)
{
	// Nothing to do yet
	printk(KERN_EMERG ">> switched_from_lottery() called for task %s\n", p->comm);
}

static void prio_changed_lottery(struct rq *rq, struct task_struct *p,
				 int oldprio)
{
	// Nothing to do yet
	printk(KERN_EMERG ">> prio_changed_lottery() called for task %s\n", p->comm);
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