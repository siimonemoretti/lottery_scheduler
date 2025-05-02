/*
 * Lottery scheduling class implementation (Mapped to SCHED_LOTTERY policy)
 */
#include "sched.h"


static void enqueue_task_lottery(struct rq *rq, struct task_struct *p,
				 int flags)
{
   // Enqueue the task into the lottery scheduling queue
   
   // Increment the number of running tasks
   rq->ltr->nr_running++;
}

static void dequeue_task_lottery(struct rq *rq, struct task_struct *p,
              int flags)
{
   // Dequeue the task from the lottery scheduling queue

   // Decrement the number of running tasks
   rq->ltr->nr_running--;
}

static void yield_task_lottery(struct rq *rq)
{

}

static void check_preempt_curr_lottery(struct rq *rq,
               struct task_struct *p, int flags)
{}

static struct task_struct *pick_next_task_lottery(struct rq *rq)
{
   return NULL;
}

/* This function is called when a task is scheduled out.
 * It is used to update the task's lottery ticket count.
 */
static void put_prev_task_lottery(struct rq *rq, struct task_struct *p)
{}

/* This function is called when a task is scheduled in.
 * It is used to set the next task's lottery ticket count.
 */
static void set_next_task_lottery(struct rq *rq, struct task_struct *p,
               bool first)
{}

/* This function is called on each tick of the scheduler.
 * It is used to update the task's lottery ticket count and
 * check if it should be scheduled out.
 */
static void task_tick_lottery(struct rq *rq, struct task_struct *p, int queued)
{}

DEFINE_SCHED_CLASS(lottery)
{
	.enqueue_task = enqueue_task_lottery,
   .dequeue_task = dequeue_task_lottery,
   .yield_task = yield_task_lottery,

   .check_preempt_curr = check_preempt_curr_lottery,

   .pick_next_task = pick_next_task_lottery,
   .put_prev_task = put_prev_task_lottery,
   .set_next_task = set_next_task_lottery,
   .task_tick = task_tick_lottery,
};