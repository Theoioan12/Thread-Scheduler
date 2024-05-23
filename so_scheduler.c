// Buliga Theodor Ioan
// 323 CA

#include <stdlib.h>
#include <stdio.h>
#include <semaphore.h>
#include "so_scheduler.h"

// creating macros for the states
// could have also done with enum
#define MAXTHREADS 1024
#define NEW 0
#define READY 1
#define RUNNING 2
#define WAITING 3
#define TERMINATED 4
#define ERR -1

// thread structure
struct so_thread {
	tid_t id;
	unsigned short int state;
	unsigned short int event;
	unsigned short int remaining_time;
	unsigned int priority;
	so_handler *handler;
	sem_t t_sem;
};

// scheduler structure
struct so_scheduler {
	struct so_thread *crnt;
	unsigned short int num_threads;
	unsigned short int initialized;
	unsigned short int events;
	unsigned short int tq;
	long priority_queue_dim;
	struct so_thread **threads;
	struct so_thread **priority_queue;
	sem_t finished;
} so_scheduler;

// function to help handling the errors
unsigned short int error_checking (unsigned int io) {
	if (io >= so_scheduler.events)
		return ERR;

	else if (io > SO_MAX_NUM_EVENTS) {
		return ERR;
	}

    else if (io < 0)
        return ERR;
		
	else return 0;
}

// compare function
unsigned short int compare_function(unsigned short int a, unsigned short int b) {
	if (a - b > 0) {
		return 0;
	} else {
		return 1;
	}
}

// function to initialise the scheduler values
void init_scheduler_values (unsigned int tq, unsigned int io) {
		so_scheduler.initialized = READY;
		so_scheduler.crnt = NULL;
		so_scheduler.tq = tq;
		so_scheduler.events = io;
		so_scheduler.priority_queue_dim = NEW;
		so_scheduler.num_threads = NEW;
}

// function to update the scheduler values
void so_signal_update(unsigned short int *woke, unsigned short int i) {
	woke++;
	(so_scheduler.threads[i])->state = READY;
	(so_scheduler.threads[i])->event = SO_MAX_NUM_EVENTS;
	a_thread(so_scheduler.threads[i]);
}

// function to help with adding a thread
void a_thread_helper(struct so_thread *thread, unsigned short int final) {
	so_scheduler.priority_queue[final] = thread;
	(so_scheduler.priority_queue[final])->state = READY;
	so_scheduler.priority_queue_dim++;
}


// function to help with starting a thread
void s_thread(struct so_thread *thread)
{	
	// setting the corespondent values
	(thread)->state = RUNNING;
	(thread)->remaining_time = so_scheduler.tq;

	// removing the element from the priority priority_queue once started
	so_scheduler.priority_queue[so_scheduler.priority_queue_dim--] = NULL;

	// semaphore post
	(sem_post(&(*thread).t_sem) < 0) ? (printf("Semaphore post failed - 01\n")) : (printf(""));
}

// adding a thread to the priority_queue
void a_thread(struct so_thread *thread)
{
	// determining the thread's priority
	unsigned short int pr = 0;
    unsigned short int final = 0;
    unsigned short int ok = 0;

	if (so_scheduler.priority_queue_dim < MAXTHREADS) {
		while ((pr < so_scheduler.priority_queue_dim)) {
       
			((*so_scheduler.priority_queue[pr]).priority >= thread->priority) ? (ok = 1) : (ok = 0);
        
        	if (ok == 1) { 
            	break;
        	}
        	++pr; 
	}

    	final = pr;

		unsigned short int i = so_scheduler.priority_queue_dim;

		// adding the new thread
		while (i > final) {
			so_scheduler.priority_queue[i] = so_scheduler.priority_queue[i - 1];
			i--;
		}

		// using the helper function
		a_thread_helper(thread, final);
	}
}

// function for updating the scheduler
void up_scheduler(void)
{
	// getting the scheduler's dimension to further use it
	unsigned short int dim = so_scheduler.priority_queue_dim - 1;
	
	// checking if there are still threads in the priority priority_queue
	if (so_scheduler.priority_queue_dim < 1) {
		// if the thread is terminated
		if ((*so_scheduler.crnt).state == TERMINATED) {
			// semaphore post
			(sem_post(&so_scheduler.finished) < 0) ? (printf("Semaphore post failed - 02\n")) : (printf(""));
		} else
			(sem_post(&(*so_scheduler.crnt).t_sem) < 0) ? (printf("Semaphore post failed - 03\n")) : (printf(""));

		return;
	}

	// starting the next thread
	if (!so_scheduler.crnt) {
		so_scheduler.crnt = so_scheduler.priority_queue[dim];
		s_thread(so_scheduler.priority_queue[dim]);
		return;
	}

	// if the current state is waiting
	else if ((*so_scheduler.crnt).state == WAITING) {
		so_scheduler.crnt = so_scheduler.priority_queue[dim];
		s_thread(so_scheduler.priority_queue[dim]);
		return;
	}

	// or if the current state is terminated
	else if ((*so_scheduler.crnt).state == TERMINATED) {
		so_scheduler.crnt = so_scheduler.priority_queue[dim];
		s_thread(so_scheduler.priority_queue[dim]);
		return;
	}

	unsigned int cp = (*so_scheduler.crnt).priority;

	struct so_thread *next = so_scheduler.priority_queue[dim];

	// checking if the next one has greater priority
	if (cp = (*so_scheduler.crnt).priority < (*next).priority) {
		// adds the current thread
		a_thread(so_scheduler.crnt);

		// starts the next thread
		so_scheduler.crnt = next;
		s_thread(next);
		return;
	}

	// Round Robin
	if ((*so_scheduler.crnt).remaining_time < 1) {
		if ((*so_scheduler.crnt).priority == (*next).priority) {
			// adding the thread
			a_thread(so_scheduler.crnt);

			// starting the thread
			so_scheduler.crnt = next;
			s_thread(next);
			return;
		}

		(*so_scheduler.crnt).remaining_time = so_scheduler.tq;
	}

	// posting the semaphore
	(sem_post(&(*so_scheduler.crnt).t_sem) < 0) ? (printf("Semaphore post failed - 04\n")) : (printf(""));
}

// function to free a thread
void free_thread(struct so_thread *thread)
{
	// destroying the semaphore
    (sem_destroy(&(*thread).t_sem) < 0) ? (printf("Semaphore destroy failed - 01\n")) : (printf(""));

	// freeing the memory
	free(thread);
}

// start routine function
void *s_routine(void *arg)
{
	// getting the argument in a thread
	struct so_thread *thread;
	thread = (struct so_thread *) arg;

	// setting the semaphore to wait
	(sem_wait(&(*thread).t_sem) < 0) ? (printf("Semaphore wait failed - 01\n")) : (printf(""));

	// setting the corespondent values
	(thread)->handler((thread)->priority);
	(thread)->state = TERMINATED;

	// updating the scheduler
	up_scheduler();

	return 0;
}

// function to create a new thread
struct so_thread *new_thread(so_handler *func, unsigned int priority)
{
	// allocating the memory with it's check
	struct so_thread *thread = malloc(sizeof(*thread));
	(!thread) ?  (printf("Allocating memory for thread failed - 01\n")) : (printf(""));

	// intitalising thread's fields
	thread->id = INVALID_TID;
	thread->event = SO_MAX_NUM_EVENTS;
	thread->state = NEW;
	thread->handler = func;
	thread->priority = priority;
	thread->remaining_time = so_scheduler.tq;

	// initialising the semaphore
	(sem_init(&(*thread).t_sem, 0, 0) < 0) ? (printf("Semaphore initialization failed - 01\n")) : (printf(""));

	// creating the thread
	(pthread_create(&(*thread).id, NULL, &s_routine, (void *)thread) < 0) ? (printf("Pthread creation failed - 01\n")) : (printf(""));

	// returning the new thread
	return thread;
}

// so_init function
int so_init(unsigned int tq, unsigned int io)
{
	// checking for errors
	// checking if it is initialized
	if (so_scheduler.initialized != 0)
		return ERR;

	// if the maximum events was reached
	else if (io > SO_MAX_NUM_EVENTS) 
		return ERR;

	// if the time quantum expired
	else if (tq < 1) 
		return ERR;

	else {

		// initialising the scheduler
		init_scheduler_values(tq, io);

		// tried to dinamically allocate the memory, but I face some errors
		so_scheduler.priority_queue = malloc (sizeof(struct so_thread) * MAXTHREADS);
		so_scheduler.threads = malloc (sizeof(struct so_thread) * MAXTHREADS);

		// also intialising the semaphore
		(sem_init(&so_scheduler.finished, 0, 1) < 0) ? (printf("Semaphore initialization failed - 02\n")) : (printf(""));
		return 0;
	}

	// otherwise returing error
	return ERR;
}

// so_fork function
tid_t so_fork(so_handler *func, unsigned int priority)
{
	// checking for errors
	if (!func)
		return INVALID_TID;

	// checking the priority	
	else if (priority > SO_MAX_PRIO) {
		return INVALID_TID;
	}
	// for the first thread, the semaphore will wait
	else if (!so_scheduler.num_threads) {
		(sem_wait(&so_scheduler.finished) < 0) ? (printf("Semaphore wait failed - 01\n")) : (printf(""));
	} 

	// creating the thread to be returned
	struct so_thread *thread_ret = new_thread(func, priority);

	// adding the thread to the scheduler
	a_thread(thread_ret);
	so_scheduler.threads[so_scheduler.num_threads] = thread_ret;
	so_scheduler.num_threads++;

	// updating the scheduler
	(!so_scheduler.crnt) ? (up_scheduler()) : so_exec();
	
	// finally, returning it's id
	return thread_ret->id;
}

// so_wait function
int so_wait(unsigned int io)
{
	// checking for errors
	// if the maximum events was reached
	if (io >= so_scheduler.events)
		return ERR;
	
	else if (io > SO_MAX_NUM_EVENTS) {
		return ERR;
	}

	// if the parameter sent is less than 0
    else if (io < 0)
        return ERR;

	else {
	// setting the thread to wait
		(so_scheduler.crnt)->event = io;
		(so_scheduler.crnt)->state = WAITING;

		so_exec();
		return 0;
	}

	// otherwise, returning error
	return ERR;
}

// so_exec function
void so_exec(void)
{
	// an auxiliary thread to mentain the current one
	struct so_thread *tmp;
	tmp = so_scheduler.crnt;

	// decrementing the time
	(so_scheduler.crnt)->remaining_time--;

	// updating the scheduler
	up_scheduler();

	// setting the semaphore to wait
	(sem_wait(&(*tmp).t_sem) < 0) ? (printf("Semaphore wait failed - 03\n")) : (printf(""));
}

// so_end function
void so_end(void)
{
	// checking for errors
	if (so_scheduler.initialized == 0)
		return;

	else {
		// setting the semaphore to wait
		(sem_wait(&so_scheduler.finished) < 0) ? (printf("Semaphore wait failed - 04\n")) : (printf(""));

	// freeing a thread
		unsigned short int i = 0;
		while (i < so_scheduler.num_threads) {
			(pthread_join((*so_scheduler.threads[i]).id, NULL) < 0) ? (printf("Pthread join failed - 01\n")) : (free_thread(so_scheduler.threads[i]));

			i++;
		}

		// destroying the semaphore
		so_scheduler.initialized = 0;
		(sem_destroy(&so_scheduler.finished) < 0) ? (printf("Semaphore destroy failed - 02\n")) : (printf(""));
	}
}

// so_signal
int so_signal(unsigned int io)
{
	// error checking
	// if the maximum was reached
	if (io >= so_scheduler.events)
		return ERR;

	else if (io > SO_MAX_NUM_EVENTS) {
		return ERR;
	}

	// if it is less than 0
    else if (io < 0)
        return ERR;

	unsigned short int woke = 0;

	// waking the threads
	unsigned short int i = 0;
	while (i < so_scheduler.num_threads) {
		
		// setting the values
		if ((so_scheduler.threads[i])->event == io) {
			if ((so_scheduler.threads[i])->state == WAITING) {
				// increasing the number of woke and setting the fields
				woke++;
				(so_scheduler.threads[i])->state = 1;
				(so_scheduler.threads[i])->event = 256;
				a_thread(so_scheduler.threads[i]);
			}
		}
		i++;
	}

	so_exec();

	return woke;
}