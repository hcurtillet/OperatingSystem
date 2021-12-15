#include <stdlib.h>
#include <stdio.h>
#include <ucontext.h>
#include <assert.h>
#include "green.h"
#include <assert.h>

#include <signal.h>
#include <sys/time.h>
#define PERIOD 100

static sigset_t block;



#define FALSE 0
#define TRUE 1

#define STACK_SIZE 4096

static ucontext_t main_cntx = {0};
static green_t main_green = {&main_cntx, NULL, FALSE};
static green_t *running = &main_green;

static green_t *head = NULL;
static green_t *tail = NULL;

void enq(green_t * thread){
    if(thread == NULL){
        return;
    }
    if(head == NULL){
        head = thread;
        tail = thread;
        thread->next = NULL;
    }
    else{
        tail->next = thread;
        tail = thread;
    }
}

green_t * deq(){
    if(head == NULL){
        return NULL;
    }
    green_t * result = head;
    if(head == tail){
        head = NULL;
        tail = NULL;
    }
    else{
        head = head->next;
        result->next = NULL;
    }
    return result;

}

static void init() __attribute__((constructor));

void init(){
    getcontext(&main_cntx);

    sigemptyset(&block);
    sigaddset(&block, SIGVTALRM);

    struct sigaction act = {0};
    struct timeval interval;
    struct itimerval period;

    act.sa_handler = timer_handler;
    assert(sigaction(SIGVTALRM, &act, NULL) == 0);

    interval.tv_sec = 0;
    interval.tv_usec = PERIOD;
    period.it_interval = interval;
    period.it_value = interval;
    setitimer(ITIMER_VIRTUAL, &period, NULL);
}

void green_thread(){
    green_t * this = running;
    void * result = (*this->fun)(this->arg);

    sigprocmask(SIG_BLOCK, &block , NULL);
    // place waiting (joining) thread in the ready queue
    enq(this->join);
    // save result of execution 
    this->retval = result;

    // we're a zombie
    this->zombie = TRUE;

    // find the next to run

    running = deq();
    setcontext(running->context);
    sigprocmask(SIG_UNBLOCK, &block, NULL);
}

int green_create(green_t * new, void*(*fun)(void*), void * arg){
    ucontext_t * cntx = (ucontext_t*)malloc(sizeof(ucontext_t));
    getcontext(cntx);
    void * stack = malloc(STACK_SIZE);

    cntx->uc_stack.ss_sp = stack;
    cntx->uc_stack.ss_size = STACK_SIZE;
    makecontext(cntx, green_thread, 0);

    new->context = cntx;
    new->fun = fun;
    new->arg = arg;
    new-> next = NULL;
    new->retval = NULL;
    new->zombie = FALSE;

    // add new to the ready queue
    enq(new);
    return 0;
}

int green_yield(){
    sigprocmask(SIG_BLOCK, &block, NULL);
    green_t * susp = running;
    // add susp to ready queue
    enq(susp);
    // select the next thread for exectution 
    running = deq();
    swapcontext(susp->context, running->context);
    sigprocmask(SIG_UNBLOCK, &block, NULL);
    return 0;

}

int green_join(green_t  * thread, void ** val){
    sigprocmask(SIG_BLOCK, &block , NULL);
    if(!thread->zombie){
        green_t * susp = running;
        // add as the joining thread
        thread->join = susp;
        //select the next thread for the execution
        

        running = deq();
        swapcontext(susp->context, running->context);

    }
    // collect result 
    if(val!=NULL){
        *val = thread->retval;
    }
    // free context
    free(thread->context);
    sigprocmask(SIG_UNBLOCK, &block, NULL);
    return 0;
}

void green_cond_init(green_cond_t *cond){
    cond->thread = NULL;
}


void green_cond_wait(green_cond_t *cond){
    sigprocmask(SIG_BLOCK, &block , NULL);
    green_t * current = running;
    if(cond->thread == NULL){
        cond->thread = current;
        cond->thread->next = NULL;
    }
    else{
        current->next = cond->thread;
        cond->thread = current;
    }
    running = deq();
    swapcontext(current->context, running->context);
    sigprocmask(SIG_UNBLOCK, &block, NULL);
}


void green_cond_signal(green_cond_t *cond){
    sigprocmask(SIG_BLOCK, &block , NULL);
    if(cond->thread == NULL){
        return;
    }
    else{
        green_t * new = cond->thread;
        cond->thread = cond->thread->next;
        new->next = NULL;
        enq(new);
    }
    sigprocmask(SIG_UNBLOCK, &block, NULL);
}


void timer_handler(int sig){
    green_t * susp = running;

    // add the running thread to the ready queue
    enq(susp);

    // find the next for the execution 
    running = deq();
    swapcontext(susp->context, running->context);
}

void green_mutex_init(green_mutex_t * mutex){
    mutex->taken = FALSE;
    mutex->head = NULL;
    mutex->tail = NULL;
}
void green_mutex_lock(green_mutex_t * mutex){
    sigprocmask(SIG_BLOCK, &block, NULL);
    printf("Start lock\n");
    if(mutex->taken == FALSE){ // we take the mutex
        mutex->taken = TRUE;
    }
    else{ // we lock the thread and place it into the mutex queue 
        int i = *(int*)running->arg;
        if(i == 3){
            printf("Locking the tread: %d \n", i);
        }
        green_t * current = running;
        if(mutex->head == NULL){
            mutex->head = current;
            mutex->tail = current;
        }
        else{
            mutex->tail->next = current;
            mutex->tail = current;
        }
        running = deq();
        if(running == NULL){
            printf("Problem here\n");
        }
        swapcontext(current->context, running->context);
    }
    printf("End lock\n");
    sigprocmask(SIG_UNBLOCK, &block, NULL);
}
void green_mutex_unlock(green_mutex_t * mutex){
    sigprocmask(SIG_BLOCK, &block, NULL);
    printf("Start unlock\n");
    if(mutex->head == NULL){ // no other thread waiting for the mutex
        mutex->taken = FALSE;
    }
    else{ // some threads are waiting for the mutex
        enq(mutex->head);
        mutex->head = mutex->head->next;
    }
    printf("End unlock\n");
    sigprocmask(SIG_UNBLOCK, &block, NULL);
}