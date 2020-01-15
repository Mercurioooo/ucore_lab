#include <defs.h>
#include <list.h>
#include <proc.h>
#include <assert.h>
#include <default_sched.h>

//初始化结构体run_queue中的run_list队列，并初始化进程数为0：
//这里出现的run_queue是一个结构体，调度框架sched_class的操作对象就是它。其中包含了运行队列（双向链表）、进程数、最大时间片和堆实现的运行进程池。
static void
RR_init(struct run_queue *rq) {
    list_init(&(rq->run_list));
    rq->proc_num = 0;
}

//RR_enqueue函数作用是把一个进程放到run_queue中，并把进程数加1
static void
RR_enqueue(struct run_queue *rq, struct proc_struct *proc) {
    assert(list_empty(&(proc->run_link)));
    list_add_before(&(rq->run_list), &(proc->run_link));
    if (proc->time_slice == 0 || proc->time_slice > rq->max_time_slice) {
        proc->time_slice = rq->max_time_slice;
    }
    proc->rq = rq;
    rq->proc_num ++;
}
//RR_dequeue函数作用是从run_queue队首取出一个进程，并把进程数减1：
static void
RR_dequeue(struct run_queue *rq, struct proc_struct *proc) {
    assert(!list_empty(&(proc->run_link)) && proc->rq == rq);
    list_del_init(&(proc->run_link));
    rq->proc_num --;
}
//这个函数地作用是选择下一个进程，由于RR算法的调度次序实际上就是FIFO调度算法，因此该函数就是简单地即选取就绪进程队列rq中的队首进程，并把队首进程转换成进程控制块指针：
static struct proc_struct *
RR_pick_next(struct run_queue *rq) {
    list_entry_t *le = list_next(&(rq->run_list));
    if (le != &(rq->run_list)) {
        return le2proc(le, run_link);
    }
    return NULL;
}
//每次时钟周期，当前执行进程的时间片time_slice减1。如果time_slice为0，则设置此进程为need_resched。这样下次中断到达时，schedule函数就会将其放在就绪队列尾部。
static void
RR_proc_tick(struct run_queue *rq, struct proc_struct *proc) {
    if (proc->time_slice > 0) {
        proc->time_slice --;
    }
    if (proc->time_slice == 0) {
        proc->need_resched = 1;
    }
}

/*
struct sched_class default_sched_class = {
    .name = "RR_scheduler",
    .init = RR_init,
    .enqueue = RR_enqueue,
    .dequeue = RR_dequeue,
    .pick_next = RR_pick_next,
    .proc_tick = RR_proc_tick,
};
*/
