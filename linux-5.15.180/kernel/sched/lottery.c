/*
 * Lottery scheduling class implementation (Mapped to SCHED_LOTTERY policy)
 */
#include "sched.h"
#include "pelt.h"
#include <linux/printk.h>

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
	struct sched_ltr_entity *ltr_se = &p->ltr;
   struct ltr_rq *ltr_rq = rq->ltr;

   list_add_tail(&ltr_se->run_list, &ltr_rq->queue);

	// Increment the number of running tasks
	rq->ltr->nr_running++;
   ltr_rq->total_tickets += ltr_se->tickets;
}

/*
 * When a task is no longer runnable, it is removed from the lottery queue.
 * And it decrements the nr_running and total_tickets in the runqueue.
 */
static void
dequeue_task_lottery(struct rq *rq, struct task_struct *p,int flags)
{
	// Dequeue the task from the lottery scheduling queue
   struct sched_ltr_entity *ltr_se = &p->ltr;
   struct ltr_rq *ltr_rq = rq->ltr;

   list_del(&ltr_se->run_list);
	rq->ltr->nr_running--;
   ltr_rq->total_tickets -= ltr_se->tickets;
}

static void yield_task_lottery(struct rq *rq)
{
}

/*
 * This function checks if a task that entered the runnable state should preempt the currently running task.
 */
static void check_preempt_curr_lottery(struct rq *rq, struct task_struct *p,
				       int flags)
{
}

/*
 * This function randomly picks the next task to run based on the lottery ticket system.
 * It's the task that should run NOW, not the NEXT task.
 */
static struct task_struct *pick_next_task_lottery(struct rq *rq)
{
	return NULL;
}

static void put_prev_task_lottery(struct rq *rq, struct task_struct *p)
{
}

static void set_next_task_lottery(struct rq *rq, struct task_struct *p,
				  bool first)
{
}

/*
 * This function is mostly called from the time tick functions. Its what drives the preemption.
 * p is the task that is currently running.
 * rq is the runqueue that the task is on.
 */
static void task_tick_lottery(struct rq *rq, struct task_struct *p, int queued)
{
}

DEFINE_SCHED_CLASS(lottery){
	.enqueue_task = enqueue_task_lottery,
	.dequeue_task = dequeue_task_lottery,
	.yield_task = yield_task_lottery,

	.check_preempt_curr = check_preempt_curr_lottery,

	.pick_next_task = pick_next_task_lottery,
	.put_prev_task = put_prev_task_lottery,
	.set_next_task = set_next_task_lottery,
	.task_tick = task_tick_lottery,
};