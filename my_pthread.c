// File:	my_pthread.c
// Author:	Yujie REN
// Date:	09/23/2017

// name: Lance Fletcher, Jeremy Banks, Christopher Gong
// username of iLab: laf224
// iLab Server: composite.cs.rutgers.edu

#include "my_pthread_t.h"

#define STACK_S (8 * 1024) //8kB stack frames
#define READY 0
#define YIELD 1
#define WAIT 2
#define EXIT 3
#define JOIN 4
#define MUTEX_WAIT 5
#define MAX_SIZE 15
#define INTERVAL 20000

//-----------------------------
#define MEM_SIZE (1024 * 1024 * 8)
#define DISK_SIZE (1024 * 1024 * 16)
#define PAGE_SIZE (1024 * 4)
#define MAX_PAGES (1024 * 1024 * 8)/(1024 * 4)
#define MEM_SECTION (MEM_SIZE/2)
#define META_SWAP_S (DISK_SIZE/PAGE_SIZE)
#define META_PHYS_S ((MEM_SIZE/PAGE_SIZE)/2)
#define CLEAR_FLAG 0
#define CREATE_PAGE 1
#define CONTEXT_SWITCH 2
#define EXTEND_PAGES 3
#define FREE_FRAMES 4
<<<<<<< HEAD
=======
#define EXIT_ERROR 5
>>>>>>> why god why
//-----------------------------


tcb *currentThread, *prevThread;
list *runningQueue[MAX_SIZE];
list *allThreads[MAX_SIZE];
ucontext_t cleanup;
sigset_t signal_set;
mySig sig;

//------------------------------------------------------
/*MEMORY GLOBALS*/
static unsigned char * PHYS_MEMORY;//Physical Memory (obviously)
static frameMeta frameMetaPhys[META_PHYS_S];//metadata for physical memory
static frameMeta frameMetaSwap[META_SWAP_S];//metadata for swap file
short pageFlag;//0 = no error, 1 = malloc-request, 2 free coalescing
size_t request_size;//requested size from malloc
unsigned char* startAddr;//on a malloc, address of a already allocated header, but we need to now actually make all its frame metas
<<<<<<< HEAD
=======
unsigned char* endAddr;//for free only, used in conjuction with startAddr to decide how many pages to free
>>>>>>> why god why
FILE* swapfile;//disk space to hold page frames
int isSet;//flag to initialize memory, disk, and metadata
int swapFileFD;//Swap file file descriptor
int blockToFreeSize;//used in the signal handler to find out how many pages we need to free (frame meta wise)


//-----------------------------------------------------

struct itimerval timer, currentTime;

int mainRetrieved;
int timeElapsed;
int threadCount;
int notFinished;


//L: Signal handler to reschedule upon VIRTUAL ALARM signal
void scheduler(int signum)
{
<<<<<<< HEAD
  printf("Checking Scheduler...\n");
  if(notFinished)
  {
    printf("caught in the handler! Get back!\n");
    return;
  }

=======
  //printf("In Scheduler~Current TID: %d\tStatus: %d\t PageFlag: %d\n",currentThread->tid,currentThread->status, pageFlag);
  if(notFinished)
  {
    //printf("caught in the handler! Get back!\n");
    //L: reset timer
    timer.it_value.tv_sec = 0;
    timer.it_value.tv_usec = INTERVAL * (currentThread->priority + 1);
    timer.it_interval.tv_sec = 0;
    timer.it_interval.tv_usec = 0;
    int ret = setitimer(ITIMER_VIRTUAL, &timer, NULL);
    if (ret < 0)
    {
      //////printf("Timer Reset Failed. Exiting...\n");
      exit(0);
    }
    return;
  }
  //printf("I'm back!\n");
>>>>>>> why god why
  //Record remaining time
  getitimer(ITIMER_VIRTUAL, &currentTime);


  //AT THIS POINT THE CURRENT THREAD IS NOT IN THE SCHEDULER (running queue, but it's always in allthreads)
  //once the timer finishes, the value of it_value.tv_usec will reset to the interval time (note this was when we set it_interval only)
<<<<<<< HEAD
  //printf("\n[Thread %d] Signaled from %d, time left %i\n", currentThread->tid,currentThread->tid, (int)currentTime.it_value.tv_usec);
=======
  //////printf("\n[Thread %d] Signaled from %d, time left %i\n", currentThread->tid,currentThread->tid, (int)currentTime.it_value.tv_usec);
>>>>>>> why god why

  //L: disable timer if still activehttps://www.google.com/search?q=complete+v
  timer.it_value.tv_sec = 0;
  timer.it_value.tv_usec = 0;
  timer.it_interval.tv_sec = 0;
  timer.it_interval.tv_usec = 0;
  setitimer(ITIMER_VIRTUAL, &timer, NULL);

  if(signum != SIGVTALRM)
  {
<<<<<<< HEAD
    //printf("[Thread %d] Signal Received: %d.\nExiting...\n", currentThread->tid, signum);
=======
    //////printf("[Thread %d] Signal Received: %d.\nExiting...\n", currentThread->tid, signum);
>>>>>>> why god why
    exit(signum);
  }


  //L: Time elapsed = difference between max interval size and time remaining in timer
  //if the time splice runs to completion the else body goes,
  //else the if body goes, and the actual amount of time that passed is added to timeelapsed
  int timeSpent = (int)currentTime.it_value.tv_usec;
  int expectedInterval = INTERVAL * (currentThread->priority + 1);
<<<<<<< HEAD
  //printf("timeSpent: %i, expectedInterval: %i\n", timeSpent, expectedInterval);
=======
  //////printf("timeSpent: %i, expectedInterval: %i\n", timeSpent, expectedInterval);
>>>>>>> why god why
  if(timeSpent < 0 || timeSpent > expectedInterval)
  {
    timeSpent = 0;
  }
  else
  {
    timeSpent = expectedInterval - timeSpent;
  }

  
  timeElapsed += timeSpent;
<<<<<<< HEAD
  //printf("total time spend so far before maintenance cycle %i and the amount of time spent just now %i\n", timeElapsed, timeSpent);
  //printf("[Thread %d] Total time: %d from time remaining: %d out of %d\n", currentThread->tid, timeElapsed, (int)currentTime.it_value.tv_usec, INTERVAL * (currentThread->priority + 1));
=======
  //////printf("total time spend so far before maintenance cycle %i and the amount of time spent just now %i\n", timeElapsed, timeSpent);
  //////printf("[Thread %d] Total time: %d from time remaining: %d out of %d\n", currentThread->tid, timeElapsed, (int)currentTime.it_value.tv_usec, INTERVAL * (currentThread->priority + 1));
>>>>>>> why god why

  //L: check for maintenance cycle
  if(timeElapsed >= 10000000)
  {
    //printf("\n[Thread %d] MAINTENANCE TRIGGERED\n\n",currentThread->tid);
    maintenance();

    //L: reset counter
    timeElapsed = 0;
  }

  prevThread = currentThread;
<<<<<<< HEAD
  
  int i;
  printf("right before switch case\n");
  switch(currentThread->status)
  {
    case READY: //READY signifies that the current thread is in the running queue
      printf("ready\n");
=======
  //printf("prevThread: %d\n",prevThread->tid);
  
  int i;
  ////printf("right before switch case\n");
  switch(currentThread->status)
  {
    case READY: //READY signifies that the current thread is in the running queue
      //printf("READY\n");
>>>>>>> why god why
      if(currentThread->priority < MAX_SIZE - 1)
      {
	currentThread->priority++;
      }

      //put back the thread that just finished back into the running queue
      enqueue(&runningQueue[currentThread->priority], currentThread);

      currentThread = NULL;

      for(i = 0; i < MAX_SIZE; i++) 
      {
        if (runningQueue[i] != NULL)
        { 
          //getting a new thread to run
          currentThread = dequeue(&runningQueue[i]);
	  break;
        }
	else
	{
	}
      }

      if(currentThread == NULL)
      {
        currentThread = prevThread;
      }

      break;
   
    case YIELD: //YIELD signifies pthread yield was called; don't update priority
<<<<<<< HEAD
      printf("yield\n");
=======
      //printf("YIELD\n");
>>>>>>> why god why
      currentThread = NULL;

      for (i = 0; i < MAX_SIZE; i++) 
      {
        if (runningQueue[i] != NULL)
        { 
          currentThread = dequeue(&runningQueue[i]);
	  break;
        }
      }
      
      if(currentThread != NULL)
      {
	//later consider enqueuing it to the waiting queue instead
	enqueue(&runningQueue[prevThread->priority], prevThread);
      }
      else
      {
	currentThread = prevThread;
      }

      break;

    case WAIT:
<<<<<<< HEAD
      printf("wait\n");
=======
      //printf("WAIT\n");
>>>>>>> why god why
      //L: When would something go to waiting queue?
      //A: In the case of blocking I/O, how do we detect this? Sockets
      //L: GG NOT OUR PROBLEM ANYMORE
      //enqueue(&waitingQueue, currentThread);
      
      break;

    case EXIT:
<<<<<<< HEAD
      printf("exit\n");
=======
      //printf("EXIT\n");
>>>>>>> why god why
      currentThread = NULL;

      for (i = 0; i < MAX_SIZE; i++) 
      {
        if (runningQueue[i] != NULL)
        {
          currentThread = dequeue(&runningQueue[i]);
	  break;
        }
      }

      if(currentThread == NULL)
      {
	//L: what if other threads exist but none are in running queue?
<<<<<<< HEAD
	//printf("No other threads found. Exiting\n");
=======
	////printf("No other threads found. Exiting\n");
>>>>>>> why god why

	//L: DO NOT USE EXIT() HERE. THAT IS A LEGIT TIME BOMB. ONLY USE RETURN
        return;
      }
      //L: free the thread control block and ucontext
      mydeallocate(prevThread->context->uc_stack.ss_sp, __FILE__, __LINE__, 0);
      mydeallocate(prevThread->context, __FILE__, __LINE__, 0);
      mydeallocate(prevThread, __FILE__, __LINE__, 0);
      currentThread->status = READY;

<<<<<<< HEAD
      //printf("Switching to: TID %d Priority %d\n", currentThread->tid, currentThread->priority);
=======
      //////printf("Switching to: TID %d Priority %d\n", currentThread->tid, currentThread->priority);
>>>>>>> why god why

      //L: reset timer
      timer.it_value.tv_sec = 0;
      timer.it_value.tv_usec = INTERVAL * (currentThread->priority + 1);
      timer.it_interval.tv_sec = 0;
      timer.it_interval.tv_usec = 0;
      int ret = setitimer(ITIMER_VIRTUAL, &timer, NULL);
      if (ret < 0)
      {
<<<<<<< HEAD
        //printf("Timer Reset Failed. Exiting...\n");
        exit(0);
      }
      setcontext(currentThread->context);

      break;

    case JOIN: //JOIN corresponds with a call to pthread_join
      printf("join\n");
=======
        //////printf("Timer Reset Failed. Exiting...\n");
        exit(0);
      }
      //printf("Setting Context After Exit\n");
      //printPhysicalMemory();
      setcontext(currentThread->context);
      //printf("Just Setted Context After Exit\n");
      break;

    case JOIN: //JOIN corresponds with a call to pthread_join
      //printf("JOIN\n");
>>>>>>> why god why
      currentThread = NULL;
      //notice how we don't enqueue the thread that just finished back into the running queue
      //we just go straight to getting another thread
      for (i = 0; i < MAX_SIZE; i++) 
      {
        if (runningQueue[i] != NULL)
        { 
          currentThread = dequeue(&runningQueue[i]);
	  break;
        }
      }

      if(currentThread == NULL)
      {
	/*WE'VE GOT A PROBLEM*/
	exit(EXIT_FAILURE);
      }
      
      break;
      
    case MUTEX_WAIT: //MUTEX_WAIT corresponds with a thread waiting for a mutex lock
      
      //L: Don't add current to queue: already in mutex queue
      currentThread = NULL;

      for (i = 0; i < MAX_SIZE; i++) 
      {
        if (runningQueue[i] != NULL)
        { 
          currentThread = dequeue(&runningQueue[i]);
	  break;
        }
      }

      if(currentThread == NULL)
      {
        /*OH SHIT DEADLOCK*/
<<<<<<< HEAD
        //printf("DEADLOCK DETECTED\n");
=======
        //////printf("DEADLOCK DETECTED\n");
>>>>>>> why god why
	exit(EXIT_FAILURE);
      }

      break;

    default:
<<<<<<< HEAD
      //printf("Thread Status Error: %d\n", currentThread->status);
=======
      //////printf("Thread Status Error: %d\n", currentThread->status);
>>>>>>> why god why
      exit(-1);
      break;
  }

	
  currentThread->status = READY;


  //L: reset timer to 25ms times thread priority
  timer.it_value.tv_sec = 0;
  timer.it_value.tv_usec = INTERVAL * (currentThread->priority + 1) ;
  timer.it_interval.tv_sec = 0;
  timer.it_interval.tv_usec = 0;
  int ret = setitimer(ITIMER_VIRTUAL, &timer, NULL);

  if (ret < 0)
  {
<<<<<<< HEAD
     //printf("Timer Reset Failure. Exiting...\n");
     exit(0);
  }

  //printf("Switching to: TID %d Priority %d\n", currentThread->tid, currentThread->priority);
  //Switch to new context
  if(prevThread->tid == currentThread->tid)  
  {/*Assume switching to same context is bad. So don't do it.*/}
  else
  {
    pageFlag = CONTEXT_SWITCH;
    printf("about to raise sigsegv\n");
    raise(SIGSEGV);
    pageFlag = CLEAR_FLAG;
    //reprotect memory
=======
     //////printf("Timer Reset Failure. Exiting...\n");
     exit(0);
  }

  //////printf("Switching to: TID %d Priority %d\n", currentThread->tid, currentThread->priority);
  //Switch to new context
  if(prevThread->tid == currentThread->tid)  
  {
	/*Assume switching to same context is bad. So don't do it.*/
        //printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~What does this meean?\n");
  }
  else
  {
    //printf("C-C-C-Context Switcher\n");
    pageFlag = CONTEXT_SWITCH;
    ////printf("about to raise sigsegv\n");
    raise(SIGSEGV);
    pageFlag = CLEAR_FLAG;
    
>>>>>>> why god why
    swapcontext(prevThread->context, currentThread->context);
  }

  return;
}

//L: thread priority boosting
void maintenance()
{
  int i;
  tcb *tgt;


  //L: template for priority inversion
  for(i = 1; i < MAX_SIZE; i++)
  {
    while(runningQueue[i] != NULL)
    {
      tgt = dequeue(&runningQueue[i]);
      tgt->priority = 0;
      enqueue(&runningQueue[0], tgt);
    }
  }

  return;
}

//L: handle exiting thread: supports invoked/non-invoked pthread_exit call
void garbage_collection()
{
  //L: Block signal here
<<<<<<< HEAD
  
=======
  //printf("entering garbage collection\n");
>>>>>>> why god why
  notFinished = 1;
  
  //if we havent called pthread create yet
  if(!mainRetrieved)
  {
    exit(EXIT_SUCCESS);
  }

  currentThread->status = EXIT;

  int i;
  for(i = 0; i < META_PHYS_S; i++)
  {
<<<<<<< HEAD
    if(frameMetaPhys[i].owner == currentThread->tid)
    {
=======
    if(frameMetaPhys[i].owner == currentThread->tid && !frameMetaPhys[i].isFree)
    {
      //printf("Hi, I'm Thread#%d, you may remember me from such pages as %i\n",currentThread->tid,i);
>>>>>>> why god why
      bzero(&PHYS_MEMORY[(frameMetaPhys[i].pageNum * PAGE_SIZE) + MEM_SECTION], PAGE_SIZE);
      mprotect(&PHYS_MEMORY[(frameMetaPhys[i].pageNum * PAGE_SIZE) + MEM_SECTION], PAGE_SIZE, PROT_NONE);

      frameMetaPhys[i].isFree = 1;
      frameMetaPhys[i].owner = 0;
      frameMetaPhys[i].pageNum = 0;
    }

  }


  tcb *jThread = NULL; //any threads waiting on the one being garbage collected

  //L: dequeue all threads waiting on this one to finish
  while(currentThread->joinQueue != NULL)
  {
    jThread = l_remove(&currentThread->joinQueue);
    jThread->retVal = currentThread->jVal;
    enqueue(&runningQueue[jThread->priority], jThread);
  }

  //L: free stored node in allThreads
  int key = currentThread->tid % MAX_SIZE;
  if(allThreads[key]->thread->tid == currentThread->tid)
  {
    list *removal = allThreads[key];
    allThreads[key] = allThreads[key]->next;
    mydeallocate(removal, __FILE__, __LINE__, 0);
  }

  else
  {
    list *temp = allThreads[key];
    while(allThreads[key]->next != 0)
    {
      if(allThreads[key]->next->thread->tid == currentThread->tid)
      {
	list *removal = allThreads[key]->next;
	allThreads[key]->next = removal->next;
	mydeallocate(removal, __FILE__, __LINE__, 0);
        break;
      }
      allThreads[key] = allThreads[key]->next;
    }

    allThreads[key] = temp;
  }
<<<<<<< HEAD

=======
  //printf("garbage collection finished haha yea right\n");
  pageFlag = CLEAR_FLAG;
>>>>>>> why god why
  notFinished = 0;

  raise(SIGVTALRM);
}

//L: add to queue
void enqueue(list** q, tcb* insert)
{
  list *queue = *q;

  if(queue == NULL)
  {
    queue = (list*)myallocate(sizeof(list), __FILE__, __LINE__, 0);
    if(queue == NULL)
    {
      return; //TODO:is this safe
    }
    queue->thread = insert;
    queue->next = queue;
    *q = queue;
    return;
  }

  list *front = queue->next;
  queue->next = (list*)myallocate(sizeof(list), __FILE__, __LINE__, 0);
  if(queue->next == NULL)
  {
    return;
  }
  queue->next->thread = insert;
  queue->next->next = front;

  queue = queue->next;
  *q = queue;
  return;
}

//L: remove from queue
tcb* dequeue(list** q)
{
  list *queue = *q;
  if(queue == NULL)
  {
    return NULL;
  }
  //queue is the last element in a queue at level i
  //first get the thread control block to be returned
  list *front = queue->next;
  tcb *tgt = queue->next->thread;
  //check if there is only one element left in the queue
  //and assign null/free appropriately
  if(queue->next == queue)
  { 
    queue = NULL;
  }
  else
  {
    queue->next = front->next;
  }
  mydeallocate(front, __FILE__, __LINE__, 0);

  
  if(tgt == NULL)
<<<<<<< HEAD
  {//printf("WE HAVE A PROBLEM IN DEQUEUE\n");
=======
  {//////printf("WE HAVE A PROBLEM IN DEQUEUE\n");
>>>>>>> why god why
  }

  *q = queue;
  return tgt;
}

//L: insert to list
void l_insert(list** q, tcb* jThread) //Non-circular Linked List
{
  list *queue = *q;
  if(queue == NULL)
  {
<<<<<<< HEAD
    //printf("if queue is null in l_insert\n");
    //printPhysicalMemory();
    queue = (list*)myallocate(sizeof(list),__FILE__, __LINE__, 0);
    //printPhysicalMemory();
    //printf("finished allocating for list\n");
=======
    //////printf("if queue is null in l_insert\n");
    //printPhysicalMemory();
    queue = (list*)myallocate(sizeof(list),__FILE__, __LINE__, 0);
    //printPhysicalMemory();
    //////printf("finished allocating for list\n");
>>>>>>> why god why
    if(queue == NULL)
    {
      return;
    }
    queue->thread = jThread;
    queue->next = NULL;
    *q = queue;
    return;
  }
<<<<<<< HEAD
  //printf("after checking if queue is null in l_insert2\n");
=======
  //////printf("after checking if queue is null in l_insert2\n");
>>>>>>> why god why
  list *newNode = (list*)myallocate(sizeof(list), __FILE__, __LINE__, 0);
  if(newNode == NULL)
  {
    return;
  }

  newNode->thread = jThread;

  //L: append to front of LL
  newNode->next = queue;
  
  queue = newNode;
  *q = queue;
  return;
}

//L: remove from list
tcb* l_remove(list** q)
{
  list *queue = *q;

  if(queue == NULL)
  {
    return NULL;
  }

  list *temp = queue;
  tcb *ret = queue->thread;
  queue = queue->next;
  mydeallocate(temp, __FILE__, __LINE__, 0);
  *q = queue;
  return ret;
}


//L: Search table for a tcb given a uintint page_request = ceil((double)(size_req_int)/ PAGE_SIZE);
tcb* thread_search(my_pthread_t tid)
{
  int key = tid % MAX_SIZE;
  tcb *ret = NULL;

  list *temp = allThreads[key];
  while(allThreads[key] != NULL)
  {
    if(allThreads[key]->thread->tid == tid)
    {
      ret = allThreads[key]->thread;
      break;
    }
    allThreads[key] = allThreads[key]->next;
  }

  allThreads[key] = temp;

  return ret;
}

void initializeMainContext()
{
  tcb *mainThread = (tcb*)myallocate(sizeof(tcb), __FILE__, __LINE__, 0);
  ucontext_t *mText = (ucontext_t*)myallocate(sizeof(ucontext_t), __FILE__, __LINE__, 0);
  if(mainThread == NULL || mText == NULL)
  {
    return;
  }
<<<<<<< HEAD
  //printf("Getting Context\n");
  getcontext(mText);
  //printf("After get context\n");
=======
  //////printf("Getting Context\n");
  getcontext(mText);
  //////printf("After get context\n");
>>>>>>> why god why
  mText->uc_link = &cleanup;
  mainThread->context = mText;
  mainThread->tid = 0;
  mainThread->priority = 0;
  mainThread->joinQueue = NULL;
  mainThread->jVal = NULL;
  mainThread->retVal = NULL;
  mainThread->status = READY;
  
  mainRetrieved = 1;
  l_insert(&allThreads[0], mainThread);

  currentThread = mainThread;
}

void initializeGarbageContext()
{
<<<<<<< HEAD

=======
  //printf("initializing garbage collection\n");
>>>>>>> why god why
  memset(&sig,0,sizeof(mySig));
  sig.sa_handler = &scheduler;
  sigaction(SIGVTALRM, &sig,NULL);
  initializeQueues(runningQueue); //set everything to NULL
    
  //Initialize garbage collector
  getcontext(&cleanup);
  cleanup.uc_link = NULL;
  cleanup.uc_stack.ss_sp = myallocate(STACK_S, __FILE__, __LINE__, 0);
  if(cleanup.uc_stack.ss_sp == NULL)
  {
    return;
  }
  cleanup.uc_stack.ss_size = STACK_S;
  cleanup.uc_stack.ss_flags = 0;
  makecontext(&cleanup, (void*)&garbage_collection, 0);

  //L: set thread count
  threadCount = 1;
<<<<<<< HEAD
  printf("Garbage Finished~~\n");
=======
  ////printf("Garbage Finished~~\n");
>>>>>>> why god why
}

/* create a new thread */
int my_pthread_create(my_pthread_t * thread, pthread_attr_t * attr, void *(*function)(void*), void * arg)
{
<<<<<<< HEAD
  printf("thread created\n");
  int justRetrieved = 0;
  if(!mainRetrieved)
  {
    printf("Garbage---\n");
=======
  ////printf("thread created\n");
  int justRetrieved = 0;
  if(!mainRetrieved)
  {
    ////printf("Garbage---\n");
>>>>>>> why god why
    initializeGarbageContext();
    justRetrieved = 1;
  }

  notFinished = 1;
  

  //L: Create a thread context to add to scheduler
  ucontext_t* task = (ucontext_t*)myallocate(sizeof(ucontext_t), __FILE__, __LINE__, 0);
  if(task == NULL)
  {
    return -1;
  }
  getcontext(task);
<<<<<<< HEAD
  //printf("finished getting context in pthread_create\n");
  task->uc_link = &cleanup;
  //printPhysicalMemory();
  task->uc_stack.ss_sp = myallocate(STACK_S, __FILE__, __LINE__, 0);
  //printf("finished allocating stack in pthread_create\n");
=======
  //////printf("finished getting context in pthread_create\n");
  task->uc_link = &cleanup;
  //printPhysicalMemory();
  task->uc_stack.ss_sp = myallocate(STACK_S, __FILE__, __LINE__, 0);
  //////printf("finished allocating stack in pthread_create\n");
>>>>>>> why god why
  if(task->uc_stack.ss_sp == NULL)
  {
    return -1;
  }
  task->uc_stack.ss_size = STACK_S;
  task->uc_stack.ss_flags = 0;
  makecontext(task, (void*)function, 1, arg);

  tcb *newThread = (tcb*)myallocate(sizeof(tcb), __FILE__, __LINE__, 0);
  if(newThread == NULL)
  {
    return -1;
  }
  newThread->context = task;
  newThread->tid = threadCount;
  newThread->priority = 0;
  newThread->joinQueue = NULL;
  newThread->jVal = NULL;
  newThread->retVal = NULL;
  newThread->status = READY;

  *thread = threadCount;
  threadCount++;

  enqueue(&runningQueue[0], newThread);
  int key = newThread->tid % MAX_SIZE;
  l_insert(&allThreads[key], newThread);

  notFinished = 0;
<<<<<<< HEAD

=======
  ////printf("just finished the bulk of my_pthread_create\n");
>>>>>>> why god why
  //L: store main context

  if (justRetrieved)
  {
    //initializeMainContext();
<<<<<<< HEAD
    printf("Raise?\n");
    raise(SIGVTALRM);
  }
  //printf("New thread created: TID %d\n", newThread->tid);
=======
    //printf("Raise?\n");
    raise(SIGVTALRM);
  }
  //////printf("New thread created: TID %d\n", newThread->tid);
>>>>>>> why god why
  
  return 0;
};

/* give CPU pocession to other user level threads voluntarily */
int my_pthread_yield()
{
  notFinished = 1;

  if(!mainRetrieved)
  {
    initializeGarbageContext();
    initializeMainContext();
  }
  //L: return to signal handler/scheduler
  currentThread->status = YIELD;

  notFinished = 0;
  return raise(SIGVTALRM);
}

/* terminate a thread */
void my_pthread_exit(void *value_ptr)
{
  notFinished = 1;
<<<<<<< HEAD

  //printf("In pthread_exit, mainRetrieved is %i\n", mainRetrieved);
=======
  pageFlag = EXIT_ERROR;
  ////printf("In pthread_exit, mainRetrieved is %i\n", mainRetrieved);
>>>>>>> why god why
  if(!mainRetrieved)
  {
    initializeGarbageContext();
    initializeMainContext();
  }
  //L: call garbage collection
  currentThread->jVal = value_ptr;
<<<<<<< HEAD

  setcontext(&cleanup);//notFinished is reset in garbage collection
=======
  //printf("right before going to garbage collection\n");
  setcontext(&cleanup);//notFinished and pageFlag is reset in garbage collection
>>>>>>> why god why
};

/* wait for thread termination */
int my_pthread_join(my_pthread_t thread, void **value_ptr)
{
  notFinished = 1;
  if(!mainRetrieved)
  {
    initializeGarbageContext();
    initializeMainContext();
  }
  //L: make sure thread can't wait on self
  if(thread == currentThread->tid)
  {
    notFinished = 0;
    return -1;
  }

  tcb *tgt = thread_search(thread);
  
  if(tgt == NULL)
  {
    notFinished = 0;
    return -1;
  }
  
  //Priority Inversion Case
  tgt->priority = 0;

  l_insert(&tgt->joinQueue, currentThread);

  currentThread->status = JOIN;

  notFinished = 0;
  raise(SIGVTALRM);
  notFinished = 1;


  if(value_ptr == NULL)
  {
    notFinished = 0;
    return 0;
  }

  *value_ptr = currentThread->retVal;
  notFinished = 0;
  return 0;
};

/* initial the mutex lock */
int my_pthread_mutex_init(my_pthread_mutex_t *mutex, const pthread_mutexattr_t *mutexattr)
{
  notFinished = 1;
  my_pthread_mutex_t m = *mutex;
  
  m.available = 1;
  m.locked = 0;
  m.holder = -1; //holder represents the tid of the thread that is currently holding the mutex
  m.queue = NULL;

  *mutex = m;
  notFinished = 0;
  return 0;
};

/* aquire the mutex lock */
int my_pthread_mutex_lock(my_pthread_mutex_t *mutex)
{
  notFinished = 1;

  if(!mainRetrieved)
  {
    initializeGarbageContext();
    initializeMainContext();
  }

  //FOR NOW ASSUME MUTEX WAS INITIALIZED
  if(!mutex->available)
  {
    notFinished = 0;
    return -1;
  }

  while(__atomic_test_and_set((volatile void *)&mutex->locked,__ATOMIC_RELAXED))
  {
    //the reason why we reset notFinished to one here is that when coming back
    //from a swapcontext, notFinished may be zero and we can't let the operations
    //in the loop be interrupted
    notFinished = 1;
    enqueue(&mutex->queue, currentThread);
    currentThread->status = MUTEX_WAIT;
    //we need to set notFinished to zero before going to scheduler
    notFinished = 0;
    raise(SIGVTALRM);
  }
  notFinished = 1;

  if(!mutex->available)
  {
    mutex->locked = 0;
    notFinished = 0;
    return -1;
  }

  //Priority Inversion Case
  currentThread->priority = 0;
  mutex->holder = currentThread->tid;
  
  notFinished = 0;
  return 0;
};

/* release the mutex lock */
int my_pthread_mutex_unlock(my_pthread_mutex_t *mutex)
{
  notFinished = 1;

  if(!mainRetrieved)
  {
    initializeGarbageContext();
    initializeMainContext();
  }
  //NOTE: handle errors: unlocking an open mutex, unlocking mutex not owned by calling thread, or accessing unitialized mutex


  //ASSUMING mutex->available will be initialized to 0 by default without calling init
  //available in this case means that mutex has been initialized or destroyed (state variable)
  if(!mutex->available || !mutex->locked || mutex->holder != currentThread->tid)
  {
    notFinished = 0;
    return -1;
  }

  mutex->locked = 0;
  mutex->holder = -1;

  tcb* muThread = dequeue(&mutex->queue);
  
  if(muThread != NULL)
  {
    //Priority Inversion Case
    muThread->priority = 0;
    enqueue(&runningQueue[0], muThread);
  }

  notFinished = 0;
  return 0;
};

/* destroy the mutex */
int my_pthread_mutex_destroy(my_pthread_mutex_t *mutex)
{
  notFinished = 1;
  my_pthread_mutex_t m = *mutex;
  //L: prevent threads from accessing while mutex is in destruction process
  m.available = 0;
  notFinished = 0;

  //L: if mutex still locked, wait for thread to release lock
  while(m.locked)
  {
    raise(SIGVTALRM);
  }

  notFinished = 1;


  tcb *muThread;
  while(m.queue != NULL)
  {
    muThread = dequeue(&m.queue);
    enqueue(&runningQueue[muThread->priority], muThread);
  }

  *mutex = m;
  notFinished = 0;
  return 0;
};

void initializeQueues(list** runQ) 
{
  int i;
  for(i = 0; i < MAX_SIZE; i++) 
  {
    runningQueue[i] = NULL;
    allThreads[i] = NULL;
  }
  
}

/*				MEMORY MANAGEMENT
==================================================================================================================
==================================================================================================================
==================================================================================================================
==================================================================================================================
==================================================================================================================
*/


static void memory_manager(int signum, siginfo_t *si, void *ignoreMe)
{
<<<<<<< HEAD
  notFinished = 1;
  unsigned char *src_page = (unsigned char *)si->si_addr;
  //printf("si_addr is %p\n", si->si_addr);
  int src_offset = src_page - &PHYS_MEMORY[MEM_SECTION];
  int src_pageNum = (src_offset/PAGE_SIZE) + MEM_SECTION;//index in PHYS_MEMORY in page
  //printf("We are in the memory manager src_offset: %i\n", src_offset);
  if(pageFlag == CLEAR_FLAG) //thread/user or library/OS accessing invalid memory
  {
    //printf("->Segmentation Fault\n");
    pthread_exit(NULL);
  }
  else if(pageFlag == CREATE_PAGE) //first time we call malloc for any thread, SHOULD ONLY CREATE ONE, THE FIRST MOTHA F'ING PAGE
  {
    //printf("Creating first page...\n");
=======
  ////printf("entering memory manager\n");
  notFinished = 1;
  unsigned char *src_page = (unsigned char *)si->si_addr;
  //////printf("si_addr is %p\n", si->si_addr);
  int src_offset = src_page - &PHYS_MEMORY[MEM_SECTION];
  int src_pageNum = (src_offset/PAGE_SIZE) + MEM_SECTION;//index in PHYS_MEMORY in page
  //////printf("We are in the memory manager src_offset: %i\n", src_offset);
  if(pageFlag == EXIT_ERROR)
  {
    //printf("Fuck this shit\n");
    exit(-1);
  }
  else if(pageFlag == CLEAR_FLAG) //thread/user or library/OS accessing invalid memory
  {
    ////printf("->Segmentation Fault\n");
    pthread_exit(NULL);
    //////printf("Fuck this shit\n");
  }
  else if(pageFlag == CREATE_PAGE) //first time we call malloc for any thread, SHOULD ONLY CREATE ONE, THE FIRST MOTHA F'ING PAGE
  {
    ////printf("Creating first page...\n");
>>>>>>> why god why
    //check if page we are on is not owned by any thread at all (may need to swap out)
    //requires creating new pages
    //look to see if there's enough room on physical and disk to fit new pages requested
    //insert frame metas either in physical or disk for the malloc request
    //check physical memory first
    //assuming for the first malloc in a thread
<<<<<<< HEAD
    if(frameMetaPhys[0].isFree)
    {
      //if this page isn't owned by any thread
      frameMetaPhys[0].isFree = 0; 
      frameMetaPhys[0].owner = currentThread->tid;
      frameMetaPhys[0].pageNum = 0;
=======
    //printCurrentThreadMemory();
    //printf("isFree: %04x\n",frameMetaPhys[0].isFree);
    fflush(stdout);
    if(frameMetaPhys[0].isFree)
    {
      //printf("---->OKAY, how about here?\n");
      //if this page isn't owned by any thread
      frameMetaPhys[0].isFree = 0;
      //printf("One\n");
      fflush(stdout);
      frameMetaPhys[0].owner = currentThread->tid;
      //printf("Two\n");
      fflush(stdout);
      frameMetaPhys[0].pageNum = 0;
      //printf("Three\n");
      fflush(stdout);
>>>>>>> why god why
      //swapMe(0,i,i); //swap the meta we just inserted into the right spot
    }
    else if(frameMetaPhys[0].owner != currentThread->tid)
    {
      //if this page is owned by another thread, need to swap it out after inserting page to somewhere in physical mem or swap file
      //first find open page in physical or swap file
<<<<<<< HEAD
=======
      //printf("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$is it here?\n");
>>>>>>> why god why
      int j;
      int foundInPhys = 0;
      int foundInDisk = 0;
      //check phys
      for(j = 1; j < META_PHYS_S; j++)
      {
        if(frameMetaPhys[j].isFree)
        {
          frameMetaPhys[j].isFree = 0;
          frameMetaPhys[j].owner = currentThread->tid;
          frameMetaPhys[j].pageNum = 0;
          swapMe(0,0,j); //swap the meta we just inserted into the right spot
          foundInPhys = 1;
          break;
        }
      }
      //check disk
      if(!foundInPhys)
      {
        for(j = 0; j < META_SWAP_S; j++)
        {
          if(frameMetaSwap[j].isFree)
          {
            frameMetaSwap[j].isFree = 0;
            frameMetaSwap[j].owner = currentThread->tid;
            frameMetaSwap[j].pageNum = 0;
            swapMe(1,0,j); //swap the meta we just inserted into the right spot
            foundInDisk = 1;
            break;
          }
        }
        if(!foundInDisk)
        {
<<<<<<< HEAD
          //printf("Ran out of memory in both physical and disk\n");
=======
          //////printf("Ran out of memory in both physical and disk\n");
>>>>>>> why god why
          startAddr = NULL;
          notFinished = 0;
          return;
        }
      }
    }
<<<<<<< HEAD
    printf("Address: %p\n",&PHYS_MEMORY[MEM_SECTION]);
    mprotect(&PHYS_MEMORY[MEM_SECTION], PAGE_SIZE, PROT_READ | PROT_WRITE);
    printf("Yo mama has babies again\n");
=======
    //printf("################################################SUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUPPPPPPPPPP!\n");
    fflush(stdout);
    mprotect(&PHYS_MEMORY[MEM_SECTION], PAGE_SIZE, PROT_READ | PROT_WRITE);
    //create page in physical memory with initial header (need this because of fucking bzero wtffffffff)
    unsigned char freeBit = 0x80;
    int totalSize = (MEM_SECTION) - 3; //-3 because headers are 3 bytes
    PHYS_MEMORY[MEM_SECTION] = freeBit | ((totalSize >> 16) & 0x7f);
    PHYS_MEMORY[MEM_SECTION+1] = (totalSize >> 8) & 0xff;
    PHYS_MEMORY[MEM_SECTION+2] = totalSize & 0xff;
    //printf(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
    fflush(stdout);
    printCurrentThreadMemory();
>>>>>>> why god why
  }
  else if(pageFlag == EXTEND_PAGES) //a free block within a page owned by a thread in physical mem found [this is First Fit]
  {
    //need to create frame meta entries to fit size of request

    //src_page = startAddr;

    //start- variables refer to the address to the left that will be extended
    //src_page = segfault-triggerer
    int start_offset = startAddr - &PHYS_MEMORY[MEM_SECTION];
    int start_pageNum = MEM_SECTION + (start_offset/PAGE_SIZE);
    int headerSize = sizeof(char) * 3;
    int next_pageNum = src_pageNum + 1;
    unsigned int endOfPhys = META_PHYS_S + MEM_SECTION;
    int right_pageNum = 0;
    int left_pageNum = start_pageNum + 1; //these are the bounds for where we're gonna create pages
    //check if header is split among two pages
    if(next_pageNum < endOfPhys && src_page + headerSize <= &PHYS_MEMORY[next_pageNum * PAGE_SIZE])
    {
      right_pageNum = src_pageNum;
    }
    else if(next_pageNum < endOfPhys)
    {
      right_pageNum = src_pageNum;
    }
    else
    {
      right_pageNum = next_pageNum;
    }
    //look to see if there's enough room on physical and disk to fit new pages needed for request
    int i;
    //insert frame metas either in physical or disk for the malloc request
    //check physical memory first
    int leftFrameIndex = left_pageNum - MEM_SECTION;
    int rightFrameIndex = right_pageNum - MEM_SECTION;
    for(i = leftFrameIndex; i <= rightFrameIndex; i++)
    {
      if(frameMetaPhys[i].isFree)
      {
        //if this page isn't owned by any thread
        frameMetaPhys[i].isFree = 0; 
        frameMetaPhys[i].owner = currentThread->tid;
        frameMetaPhys[i].pageNum = i;
        //swapMe(i, 0); //swap the meta we just inserted into the right spot
        mprotect(&PHYS_MEMORY[(i * PAGE_SIZE) + MEM_SECTION], PAGE_SIZE, PROT_READ | PROT_WRITE);
      }
      else if(frameMetaPhys[i].owner != currentThread->tid)
      {
        //if this page is owned by another thread, need to swap it out after inserting page to somewhere in physical mem or swap file
        //first find open page in physical or swap file
        int j;
        int foundInPhys = 0;
        int foundInDisk = 0;
        //check phys
        for(j = i + 1; j < META_PHYS_S; j++)
        {
          if(frameMetaPhys[j].isFree)
          {
            frameMetaPhys[j].isFree = 0;
            frameMetaPhys[j].owner = currentThread->tid;
            frameMetaPhys[j].pageNum = i;
            swapMe(0, i, j); //swap the meta we just inserted into the right spot
            foundInPhys = 1;
            break;
          }
        }
        //check disk
        if(!foundInPhys)
        {
          for(j = 0; j < META_SWAP_S; j++)
          {
            if(frameMetaSwap[j].isFree)
            {
              frameMetaSwap[j].isFree = 0;
              frameMetaSwap[j].owner = currentThread->tid;
              frameMetaSwap[j].pageNum = i;
              swapMe(1, i, j); //swap the meta we just inserted into the right spot
              foundInDisk = 1;
              break;
            }
          }
          if(!foundInDisk)
          {
<<<<<<< HEAD
            //printf("Ran out of memory in both physical and disk\n");
=======
            //////printf("Ran out of memory in both physical and disk\n");
>>>>>>> why god why
            startAddr = NULL;
	    notFinished = 0;
            return;
          }
        }
      }
    }
  }
  else if(pageFlag == CONTEXT_SWITCH)
  {
    //printf("we're context switching by swapping pages bitchessssssssss\n");
    //find all pages that belong to current thread in physical memory
    int i;
    for(i = 0; i < META_PHYS_S; i++)
    {
      if(frameMetaPhys[i].owner == currentThread->tid && !frameMetaPhys[i].isFree)
      {
<<<<<<< HEAD
        if (i == frameMetaPhys[i].pageNum)
        {
	  printf("THis is a print statement\n");
=======
        //printf("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@Does THIS work\n");
        if (i == frameMetaPhys[i].pageNum)
        {
	  ////printf("THis is a print statement\n");
>>>>>>> why god why
          mprotect(&PHYS_MEMORY[MEM_SECTION + (i * PAGE_SIZE)], PAGE_SIZE, PROT_READ | PROT_WRITE);
	}
	else
	{
<<<<<<< HEAD
	  printf("Swap\n");
=======
	  //printf("Swap\n");
>>>>>>> why god why
          swapMe(0,frameMetaPhys[i].pageNum,i);
	}
      }
    }
    //find all pages that belong to current thread on swap file
    for(i = 0; i < META_SWAP_S; i++)
    {
      if(frameMetaSwap[i].owner == currentThread->tid)
      {
<<<<<<< HEAD
	printf("Swap\n");
=======
	////printf("Swap\n");
>>>>>>> why god why
        swapMe(1,frameMetaPhys[i].pageNum,i);
      }
    }
  }
  else if(pageFlag == FREE_FRAMES)
  {
    src_page = startAddr;
    src_offset = src_page - &PHYS_MEMORY[MEM_SECTION];
    src_pageNum = src_offset/PAGE_SIZE;
    int headerSize = sizeof(char) * 3;

    int next_pageNum = src_pageNum + 1;
    unsigned int endOfPhys = META_PHYS_S + MEM_SECTION;
    int leftPageNum = 0;
    int rightPageNum = 0;
    //check if header is split among two pages
    if(next_pageNum < endOfPhys && src_page + headerSize <= &PHYS_MEMORY[next_pageNum * PAGE_SIZE])
    {
      leftPageNum = src_pageNum;
    }
    else if(next_pageNum < endOfPhys)
    {
      leftPageNum = src_pageNum;
    }
    else{
      leftPageNum = next_pageNum;
    }
    //have to calculate right page num
<<<<<<< HEAD
    unsigned char *end_addr = src_page + (sizeof(char) * 3) + blockToFreeSize;
    int end_offset = end_addr - &PHYS_MEMORY[MEM_SECTION];
=======
    //unsigned char *end_addr = src_page + (sizeof(char) * 3) + blockToFreeSize;
    int end_offset = endAddr - &PHYS_MEMORY[MEM_SECTION];
    ////printf("end_offset %i, src_offset %i\n", end_offset, src_offset);
>>>>>>> why god why
    rightPageNum = end_offset/PAGE_SIZE;
    if(rightPageNum % PAGE_SIZE == 0 && rightPageNum != 0)
    {
      rightPageNum--;
    }
    int i;
<<<<<<< HEAD
    for(i = leftPageNum + 1; i <= rightPageNum; i++)
    {
=======
    ////printf("leftPageNum: %i, rightPageNum %i meta_phys_s %i\n", leftPageNum, rightPageNum, META_PHYS_S);
    for(i = leftPageNum + 1; i <= rightPageNum && i < META_PHYS_S; i++)
    { 
      //////printf("%i\n", i);
>>>>>>> why god why
      //unclaiming pages in physical memory if they are owned by current thread
      if(!frameMetaPhys[i].isFree && frameMetaPhys[i].owner == currentThread->tid)
      {
        frameMetaPhys[i].isFree = 1; 
        frameMetaPhys[i].owner = 0;
        frameMetaPhys[i].pageNum = 0;
        mprotect(&PHYS_MEMORY[(i * PAGE_SIZE) + MEM_SECTION], PAGE_SIZE, PROT_NONE);
      }
    }
<<<<<<< HEAD
  }
  printf("PHYS_MEMORY: %p\tMEM_SECTION: %d\n",(void*)PHYS_MEMORY,MEM_SECTION);
  printf("daddy\n");
  printf("??????\n");
  notFinished = 0;
=======
    ////printf("Why did we just try to free frames?\n");
  }
  //////printf("PHYS_MEMORY: %p\tMEM_SECTION: %d\n",(void*)PHYS_MEMORY,MEM_SECTION);
  ////printf("---End of Memory Manager---\n");

  /*TODO: Do we need this?*/
  //notFinished = 0;
>>>>>>> why god why
}

void setMem()
{

  //initialize physical memory
  PHYS_MEMORY = (unsigned char*)memalign(PAGE_SIZE, MEM_SIZE);
  //posix_memalign((void**)&PHYS_MEMORY, PAGE_SIZE, MEM_SIZE);
  bzero(PHYS_MEMORY, MEM_SIZE);

<<<<<<< HEAD
  //printf("Memalign working?\n");
=======
  //////printf("Memalign working?\n");
>>>>>>> why god why


  //set up two blocks of size 4 MB, first half is user memory, second half is OS/library memory
  //create blocks of size MEM_SECTION [MEM_SIZE/2]
  unsigned char freeBit = 0x80;
  int totalSize = (MEM_SECTION) - 3; //-3 because headers are 3 bytes
  int pIndex = MEM_SECTION;

  //Allocation metadata: leftmost bit for free bit, 23 bits for allocation size
  //first the OS memory
  PHYS_MEMORY[0] = freeBit | ((totalSize >> 16) & 0x7f);
  PHYS_MEMORY[1] = (totalSize >> 8) & 0xff;
  PHYS_MEMORY[2] = totalSize & 0xff;
<<<<<<< HEAD
  //second the user memory
  PHYS_MEMORY[pIndex] = freeBit | ((totalSize >> 16) & 0x7f);
  PHYS_MEMORY[pIndex+1] = (totalSize >> 8) & 0xff;
  PHYS_MEMORY[pIndex+2] = totalSize & 0xff;    
=======
  /*
  //second the user memory
  PHYS_MEMORY[pIndex] = freeBit | ((totalSize >> 16) & 0x7f);
  PHYS_MEMORY[pIndex+1] = (totalSize >> 8) & 0xff;
  PHYS_MEMORY[pIndex+2] = totalSize & 0xff; */   
>>>>>>> why god why

  //Initialize MetaPhys
  int i = 0;
  for(i=0; i < META_PHYS_S; i++)
  {
    frameMetaPhys[i].isFree = 1; 
    frameMetaPhys[i].owner = 0;
    frameMetaPhys[i].pageNum = 0;
  }

  //Initialize MetaSwap
  for(i=0; i < META_SWAP_S; i++)
  {
    frameMetaSwap[i].isFree = 1; 
    frameMetaSwap[i].owner = 0;
    frameMetaSwap[i].pageNum = 0;
  }

  //initialize swap file
  initializeSwapFile();
  isSet = 1;
  //protect entire memory


  //set up signal handler
  struct sigaction sa;
  sa.sa_flags = SA_SIGINFO;
  sigemptyset(&sa.sa_mask);
  sa.sa_sigaction = memory_manager;
<<<<<<< HEAD

  if(sigaction(SIGSEGV, &sa, NULL) == -1)
  {
    //printf("Fatal error setting up signal handler\n");
=======
  mprotect(&PHYS_MEMORY[MEM_SECTION], MEM_SECTION, PROT_NONE); 
  if(sigaction(SIGSEGV, &sa, NULL) == -1)
  {
    //////printf("Fatal error setting up signal handler\n");
>>>>>>> why god why
    exit(EXIT_FAILURE);    //explode!
  }

  if(!mainRetrieved)
  {
<<<<<<< HEAD
    //printf("-->Main Initializing...\n");
    initializeMainContext();
    //printf("Main Initialized-->\n");
  }

=======
    //////printf("-->Main Initializing...\n");
    initializeMainContext();
    //////printf("Main Initialized-->\n");
  }
  
>>>>>>> why god why
  
}

/*Creates the 16 MB Swap File*/
void initializeSwapFile()
{
<<<<<<< HEAD
  //printf("Swapper\n");
=======
  //////printf("Swapper\n");
>>>>>>> why god why
  char *swapper = "swapFile";
  swapFileFD = open(swapper,O_CREAT | O_RDWR | O_TRUNC, S_IRUSR | S_IWUSR);
  lseek(swapFileFD,0,SEEK_SET);
  static unsigned char swapFileBuffer[PAGE_SIZE * PAGE_SIZE];
  bzero(swapFileBuffer,PAGE_SIZE*PAGE_SIZE);
  write(swapFileFD,swapFileBuffer,PAGE_SIZE*PAGE_SIZE);
  lseek(swapFileFD,0,SEEK_SET);
}


/*Swaps frames out for one another, exact frames depend upon the context*/
//newPos = place where it's being swapped into, oldPos = location where it was before; both relative to what we were previously searching for
void swapMe(int isOnDisk, int newPos, int oldPos)
{
  //Grabbing frame in PHYS_MEMORY

  if (!isOnDisk)
  {
    mprotect(&PHYS_MEMORY[(newPos*PAGE_SIZE) + MEM_SECTION], PAGE_SIZE, PROT_READ | PROT_WRITE);
    mprotect(&PHYS_MEMORY[(oldPos*PAGE_SIZE) + MEM_SECTION], PAGE_SIZE, PROT_READ | PROT_WRITE);
    char buffer1[PAGE_SIZE];
    char buffer2[PAGE_SIZE];
    //swap out contents of physical memory
    memcpy(buffer1,&PHYS_MEMORY[(newPos*PAGE_SIZE) + MEM_SECTION],PAGE_SIZE); //what's being swapped out
    memcpy(buffer2,&PHYS_MEMORY[(oldPos*PAGE_SIZE) + MEM_SECTION],PAGE_SIZE); //what's being swapped in
    memcpy(&PHYS_MEMORY[(newPos*PAGE_SIZE) + MEM_SECTION],buffer2,PAGE_SIZE);
    memcpy(&PHYS_MEMORY[(oldPos*PAGE_SIZE) + MEM_SECTION],buffer1,PAGE_SIZE);
    //swap out contents of meta data?
    frameMeta fm1;
    frameMeta fm2;
    memcpy(&fm1, &frameMetaPhys[newPos], sizeof(fm1));
    memcpy(&fm2, &frameMetaPhys[oldPos], sizeof(fm2));
    memcpy(&frameMetaPhys[newPos], &fm2, sizeof(fm1));
    memcpy(&frameMetaPhys[oldPos], &fm1, sizeof(fm2));
    mprotect(&PHYS_MEMORY[(oldPos*PAGE_SIZE) + MEM_SECTION], PAGE_SIZE, PROT_NONE);
  }
  else
  {
    mprotect(&PHYS_MEMORY[(newPos*PAGE_SIZE) + MEM_SECTION], PAGE_SIZE, PROT_READ | PROT_WRITE);
    //swap out contents of physical memory with page found in disk
    char buffer1[PAGE_SIZE];
    char buffer2[PAGE_SIZE];
    memcpy(buffer1,&PHYS_MEMORY[(newPos*PAGE_SIZE) + MEM_SECTION],PAGE_SIZE); //what's being swapped out
    lseek(swapFileFD, oldPos * PAGE_SIZE, SEEK_SET); //what's being swapped in
    read(swapFileFD, buffer2, PAGE_SIZE);
    memcpy(&PHYS_MEMORY[(newPos*PAGE_SIZE) + MEM_SECTION],buffer2,PAGE_SIZE);
    write(swapFileFD, buffer1, PAGE_SIZE);
    //swap out contents of meta data?
    frameMeta fm1;
    frameMeta fm2;
    memcpy(&fm1, &frameMetaPhys[newPos], sizeof(fm1));
    memcpy(&fm2, &frameMetaSwap[oldPos], sizeof(fm2));
    memcpy(&frameMetaPhys[newPos], &fm2, sizeof(fm1));
    memcpy(&frameMetaSwap[oldPos], &fm1, sizeof(fm2));
  }

  return;
}

void* myallocate(size_t size_req, char *fileName, int line, char src)
{
  notFinished = 1;

<<<<<<< HEAD
  printf("Entering Malloc[%04x]\n",src);
=======
  ////printf("Entering Malloc[%04x]\n",src);
>>>>>>> why god why

  //initialize memory and disk
  if(!isSet)
  {
<<<<<<< HEAD
    printf("isSet called\n");
    setMem();
  }

  //printf("Just set memory\n");
=======
    ////printf("isSet called\n");
    setMem();
  }

  //////printf("Just set memory\n");
>>>>>>> why god why

  if(size_req <= 0 || size_req > (MEM_SECTION) - 3)//don't allow allocations of size 0; would cause internal fragmentation due to headers
  {
    notFinished = 0;
<<<<<<< HEAD
    //printf("CRAP$$$\n");
=======
    //////printf("CRAP$$$\n");
>>>>>>> why god why
    return NULL;
  }

  request_size = size_req;//set request size global to transfer to signal handler

  unsigned int start_index, bound;
  unsigned int meta; 
  unsigned int isFree, blockSize;  

  //pageFlag = 1;//tell signal handler an occurrence of segfault would have come from malloc

  //if library call
  if(src == 0)
  {
    start_index = 0;
    bound = MEM_SECTION;
<<<<<<< HEAD
    //printf("LIBRARY\n");
=======
    //printf("--->LIBRARY MALLOC\n");
>>>>>>> why god why
  }
  //if thread call
  else if(src == 1)
  {
    start_index = MEM_SECTION;
    bound = MEM_SIZE;
<<<<<<< HEAD
    printf("USER\n");
=======
    //printf("--->USER MALLOC\n");
>>>>>>> why god why
    //printPhysicalMemory();
  }
  //reeaaally shouldn't ever happen but just in case
  else
  {
    //printf("Error on source of call to malloc. Exiting...\n");
  }
   
  while(start_index < bound)
  {
<<<<<<< HEAD
    printf("Iterating in malloc main loop, start_index is %i\n", start_index);
    //extract free bit & block size from header
    pageFlag = CREATE_PAGE;
    //printf("Before Meta\n");
    meta = (PHYS_MEMORY[start_index] << 16) | (PHYS_MEMORY[start_index+1] << 8) | (PHYS_MEMORY[start_index+2]); //if we encounter this spot in memory where there's no page, we'll go to handler to create that new page
    //printf("After Meta\n");
    pageFlag = CLEAR_FLAG;
    isFree = (meta >> 23) & 0x1;
    blockSize = meta & 0x7fffff;
=======
    //////printf("Iterating in malloc main loop, start_index is %i\n", start_index);
    //extract free bit & block size from header
    pageFlag = CREATE_PAGE;
    meta = (PHYS_MEMORY[start_index] << 16) | (PHYS_MEMORY[start_index+1] << 8) | (PHYS_MEMORY[start_index+2]); //if we encounter this spot in memory where there's no page, we'll go to handler to create that new page
    meta = (PHYS_MEMORY[start_index] << 16) | (PHYS_MEMORY[start_index+1] << 8) | (PHYS_MEMORY[start_index+2]);
    pageFlag = CLEAR_FLAG;
    isFree = (meta >> 23) & 0x1;
    blockSize = meta & 0x7fffff;
    ////printf("current blockSize: %d\n",blockSize);
>>>>>>> why god why
    //valid block found
    if(isFree && blockSize >= size_req)
    {      
      int prev_index = start_index; //in the case that we dont split, we need to increase the blockSize
      //fill in metadata for allocated block
      startAddr = &PHYS_MEMORY[start_index];
<<<<<<< HEAD
      start_index += (size_req + 3);//jump to next block to place new header
=======
      start_index += size_req + 3;//jump to next block to place new header
>>>>>>> why god why
      
      // fill in metadata for new free block
      int sizeLeft = blockSize - size_req - 3;

      if(sizeLeft > 0)//only add new header if enough space remains for allocation of at least one byte
      {
        //raise(SIGSEGV);
        PHYS_MEMORY[prev_index] = (size_req >> 16) & 0x7f;
        PHYS_MEMORY[prev_index+1] = (size_req >> 8) & 0xff;
        PHYS_MEMORY[prev_index+2] = size_req & 0xff;
        pageFlag = EXTEND_PAGES;
<<<<<<< HEAD
=======
	printf("Allocating additional pages for thread...\n");
	////printf("Next index: %d\n",start_index);
>>>>>>> why god why
        PHYS_MEMORY[start_index] = 0x80 | ((sizeLeft >> 16) & 0x7f);
        PHYS_MEMORY[start_index+1] = (sizeLeft >> 8) & 0xff;
        PHYS_MEMORY[start_index+2] = sizeLeft & 0xff;
        pageFlag = CLEAR_FLAG;
      }
      else
      {
        request_size = blockSize;
        //in the event of no split, we need to give extra memory
        PHYS_MEMORY[prev_index] = (blockSize >> 16) & 0x7f;
        PHYS_MEMORY[prev_index+1] = (blockSize >> 8) & 0xff;
        PHYS_MEMORY[prev_index+2] = blockSize & 0xff;
        pageFlag = EXTEND_PAGES;
<<<<<<< HEAD
=======
	printf("Allocating additional pages for thread...\n");
>>>>>>> why god why
	PHYS_MEMORY[prev_index+(sizeof(unsigned char)*3)+blockSize] = '\0';//intended to raise SIGSEGV, even when we don't split
        pageFlag = CLEAR_FLAG;
      }
      pageFlag = CLEAR_FLAG;//reset pageFlag
      notFinished = 0;
<<<<<<< HEAD
      printf("Malloc returned an address %li\n", (startAddr + (sizeof(char)*3))-PHYS_MEMORY);
=======
      ////printf("Malloc returned an address %li\n", (startAddr + (sizeof(char)*3))-PHYS_MEMORY);
      //printPhysicalMemory();
      //printf("----------------------------------------------\n");
>>>>>>> why god why
      return startAddr + (sizeof(char) * 3);
    }
    else
    {
      //go to next subsequent header
      start_index += (blockSize + 3);
    }
  }
  
  //If reached, no valid block could be allocated
  pageFlag = CLEAR_FLAG;//reset pageFlag

  notFinished = 0;
<<<<<<< HEAD
  printf("Malloc returned null\n");
=======
  //printf("Malloc returned null\n");
  //printPhysicalMemory();
  //printf("----------------------------------------------\n");
>>>>>>> why god why
  return NULL;
}



void mydeallocate(void *ptr, char *fileName, int line, char src)
{
  notFinished = 1;

  if(ptr == NULL)//don't do anything if null pointer passed
  {   
    notFinished = 0;
    return;
  }

  unsigned char *location  = (unsigned char*)ptr;

  if(location < PHYS_MEMORY || location > &PHYS_MEMORY[MEM_SIZE - 1])
  {
    //address the user entered is not within physical memory
<<<<<<< HEAD
    printf("about raising sigsegv\n");
=======
    ////printf("about raising sigsegv\n");
>>>>>>> why god why
    raise(SIGSEGV);
  }
  int start_index;
  int bound;
  int prevBlock = -1;
  unsigned int meta, nextMeta, prevMeta;
  unsigned char isFree, nextFree, prevFree;
  unsigned int blockSize=0, nextSize, prevSize;
  unsigned char *leftoverBlock;
  if(src == 0)//library
  {
<<<<<<< HEAD
=======
    //printf("--->Library Free\n");
>>>>>>> why god why
    start_index = 0;
    bound = MEM_SECTION;

    while(start_index < bound)
    {
      if(&PHYS_MEMORY[start_index] == location)
      {
<<<<<<< HEAD
        meta = (PHYS_MEMORY[start_index] << 16) | (PHYS_MEMORY[start_index+1] << 8) | (PHYS_MEMORY[start_index+2]);
=======
        meta = (PHYS_MEMORY[start_index-3] << 16) | (PHYS_MEMORY[start_index-2] << 8) | (PHYS_MEMORY[start_index-1]);
>>>>>>> why god why
        isFree = (meta >> 23) & 0x1;
        blockSize = meta & 0x7fffff;

        if(isFree)//block has already been freed
        {
<<<<<<< HEAD
	  //printf("Attempted Double Free\n");
	  pthread_exit(NULL);
        }

        PHYS_MEMORY[start_index] = 0x80;//reset free bit

        //coalesce with next block
        if(PHYS_MEMORY[start_index] + blockSize + 3 < MEM_SIZE)
        {
  	  nextMeta = (PHYS_MEMORY[start_index + blockSize + 3] << 16) | (PHYS_MEMORY[start_index + blockSize + 4] << 8) | (PHYS_MEMORY[start_index + blockSize + 5]);
=======
	  //////printf("Attempted Double Free\n");
	  pthread_exit(NULL);
        }

        PHYS_MEMORY[start_index-3] = 0x80;//reset free bit

        //coalesce with next block
        if(start_index + blockSize < MEM_SIZE)
        {
  	  nextMeta = (PHYS_MEMORY[start_index + blockSize] << 16) | (PHYS_MEMORY[start_index + blockSize + 1] << 8) | (PHYS_MEMORY[start_index + blockSize + 2]);
>>>>>>> why god why
          nextFree = (nextMeta >> 23) & 0x1;
          nextSize = nextMeta & 0x7fffff;

	  if(nextFree)
	  {
	    blockSize += nextSize + (sizeof(char) * 3);
<<<<<<< HEAD
	    PHYS_MEMORY[start_index] = 0x80 | ((blockSize >> 16) & 0x7f);
            PHYS_MEMORY[start_index+1] = (blockSize >> 8) & 0xff;
            PHYS_MEMORY[start_index+2] = blockSize & 0xff;
=======
	    PHYS_MEMORY[start_index-3] = 0x80 | ((blockSize >> 16) & 0x7f);
            PHYS_MEMORY[start_index-2] = (blockSize >> 8) & 0xff;
            PHYS_MEMORY[start_index-1] = blockSize & 0xff;
>>>>>>> why god why
	  }

          //coalesce with prev block
          if(prevBlock != -1)
          {
<<<<<<< HEAD
	    prevMeta = (PHYS_MEMORY[prevBlock] << 16) | (PHYS_MEMORY[prevBlock+1] << 8) | (PHYS_MEMORY[prevBlock+2]);
=======
	    prevMeta = (PHYS_MEMORY[prevBlock-3] << 16) | (PHYS_MEMORY[prevBlock-2] << 8) | (PHYS_MEMORY[prevBlock-1]);
>>>>>>> why god why
            prevFree = (prevMeta >> 23) & 0x1;
            prevSize = prevMeta & 0x7fffff;

	    if(prevFree)
	    {
	      blockSize += prevSize + (sizeof(char)*3);
<<<<<<< HEAD
              PHYS_MEMORY[prevBlock] = 0x80 | ((blockSize >> 16) & 0x7f);
              PHYS_MEMORY[prevBlock+1] = (blockSize >> 8) & 0xff;
              PHYS_MEMORY[prevBlock+2] = blockSize & 0xff;
=======
              PHYS_MEMORY[prevBlock-3] = 0x80 | ((blockSize >> 16) & 0x7f);
              PHYS_MEMORY[prevBlock-2] = (blockSize >> 8) & 0xff;
              PHYS_MEMORY[prevBlock-1] = blockSize & 0xff;
>>>>>>> why god why
	    }

          }
        }
        break;
      }
      else
      {
        prevBlock = start_index;
        start_index += blockSize + (sizeof(char) * 3);
      }

    }
  }

  else //user thread
  {
<<<<<<< HEAD
=======
    //printf("--->User Free\n");
>>>>>>> why god why
    start_index = MEM_SECTION;
    bound = MEM_SIZE;

    while(start_index < bound)
    {
      if(&PHYS_MEMORY[start_index] == location)
      {
<<<<<<< HEAD
        meta = (PHYS_MEMORY[start_index] << 16) | (PHYS_MEMORY[start_index+1] << 8) | (PHYS_MEMORY[start_index+2]);
        isFree = (meta >> 23) & 0x1;
        blockSize = meta & 0x7fffff;
        leftoverBlock = &PHYS_MEMORY[start_index];
        if(isFree)//block has already been freed
        {
	  //printf("Attempted Double Free\n");
	  pthread_exit(NULL);
        }

        PHYS_MEMORY[start_index] = 0x80;//reset free bit

        //coalesce with next block
        if(PHYS_MEMORY[start_index] + blockSize + 3 < MEM_SIZE)
        {
  	  nextMeta = (PHYS_MEMORY[start_index + blockSize + 3] << 16) | (PHYS_MEMORY[start_index + blockSize + 4] << 8) | (PHYS_MEMORY[start_index + blockSize + 5]);
          nextFree = (nextMeta >> 23) & 0x1;
          nextSize = nextMeta & 0x7fffff;

	  if(nextFree)
	  {
	    blockSize += nextSize + (sizeof(char) * 3);
	    PHYS_MEMORY[start_index] = 0x80 | ((blockSize >> 16) & 0x7f);
            PHYS_MEMORY[start_index+1] = (blockSize >> 8) & 0xff;
            PHYS_MEMORY[start_index+2] = blockSize & 0xff;
=======
	meta = (PHYS_MEMORY[start_index-3] << 16) | (PHYS_MEMORY[start_index+1-3] << 8) | (PHYS_MEMORY[start_index+2-3]);
        isFree = (meta >> 23) & 0x1;
        blockSize = meta & 0x7fffff;
	////printf("currMeta: %d\n",blockSize);
        leftoverBlock = &PHYS_MEMORY[start_index-3];
        endAddr = &PHYS_MEMORY[start_index-3];
        if(isFree)//block has already been freed
        {
	  //////printf("Attempted Double Free\n");
	  pthread_exit(NULL);
        }

        PHYS_MEMORY[start_index-3] = 0x80;//reset free bit

        //coalesce with next block
        if(start_index + blockSize < MEM_SIZE)
        {
  	  nextMeta = (PHYS_MEMORY[start_index + blockSize] << 16) | (PHYS_MEMORY[start_index + blockSize + 1] << 8) | (PHYS_MEMORY[start_index + blockSize + 2]);
          nextFree = (nextMeta >> 23) & 0x1;
          nextSize = nextMeta & 0x7fffff;
	  ////printf("nextMeta: %d\n",nextSize);
           
	  if(nextFree)
	  {
            endAddr = &PHYS_MEMORY[start_index + blockSize];
	    blockSize += nextSize + (sizeof(char) * 3);
	    PHYS_MEMORY[start_index-3] = 0x80 | ((blockSize >> 16) & 0x7f);
            PHYS_MEMORY[start_index-2] = (blockSize >> 8) & 0xff;
            PHYS_MEMORY[start_index-1] = blockSize & 0xff;
>>>>>>> why god why
	  }

          //coalesce with prev block
          if(prevBlock != -1)
          {
<<<<<<< HEAD
	    prevMeta = (PHYS_MEMORY[prevBlock] << 16) | (PHYS_MEMORY[prevBlock+1] << 8) | (PHYS_MEMORY[prevBlock+2]);
=======
	    prevMeta = (PHYS_MEMORY[prevBlock-3] << 16) | (PHYS_MEMORY[prevBlock-2] << 8) | (PHYS_MEMORY[prevBlock-1]);
>>>>>>> why god why
            prevFree = (prevMeta >> 23) & 0x1;
            prevSize = prevMeta & 0x7fffff;

	    if(prevFree)
	    {
	      blockSize += prevSize + (sizeof(char)*3);
<<<<<<< HEAD
              PHYS_MEMORY[prevBlock] = 0x80 | ((blockSize >> 16) & 0x7f);
              PHYS_MEMORY[prevBlock+1] = (blockSize >> 8) & 0xff;
              PHYS_MEMORY[prevBlock+2] = blockSize & 0xff;
              leftoverBlock = &PHYS_MEMORY[prevBlock];
=======
              PHYS_MEMORY[prevBlock-3] = 0x80 | ((blockSize >> 16) & 0x7f);
              PHYS_MEMORY[prevBlock-2] = (blockSize >> 8) & 0xff;
              PHYS_MEMORY[prevBlock-1] = blockSize & 0xff;
              leftoverBlock = &PHYS_MEMORY[prevBlock-3];
>>>>>>> why god why
	    }

          }
        }
        startAddr = leftoverBlock;
        blockToFreeSize = blockSize;
        pageFlag = FREE_FRAMES;
<<<<<<< HEAD
        printf("about to raise sigsegv\n");
=======
        ////printf("Just took out: \tblockToFreeSize: %d\n",blockToFreeSize);
>>>>>>> why god why
        raise(SIGSEGV);
        pageFlag = CLEAR_FLAG;
        break;
      }
      else
      {
<<<<<<< HEAD
=======
	////printf("Skipping %d\n",blockSize);
>>>>>>> why god why
        prevBlock = start_index;
        start_index += blockSize + (sizeof(char) * 3);
      }

    }    

  }
  notFinished = 0;
}
void printPhysicalMemory()
{
<<<<<<< HEAD

=======
>>>>>>> why god why
  unsigned int meta;
  char isFree;
  int blockSize;
  int start_index = 0;
<<<<<<< HEAD
  int end_index = MEM_SIZE;
=======
  int end_index = MEM_SECTION;
>>>>>>> why god why
  while(start_index < end_index){
    meta = (PHYS_MEMORY[start_index] << 16) | (PHYS_MEMORY[start_index+1] << 8) | (PHYS_MEMORY[start_index+2]);
    isFree = (meta >> 23) & 0x1;
    blockSize = meta & 0x7fffff;
<<<<<<< HEAD
    printf("We are on start_index %i which has a block of size %i and it is free? %i\n", start_index, blockSize, isFree);
=======
    //printf("We are on start_index %i which has a block of size %i and it is free? %i\n", start_index, blockSize, isFree);
>>>>>>> why god why
    fflush(stdout);
    start_index += sizeof(char) * 3 + blockSize;
  }

}
<<<<<<< HEAD
=======
void printCurrentThreadMemory()
{
  unsigned int meta;
  char isFree;
  int blockSize;
  int start_index = MEM_SECTION;
  //int end_index = MEM_SIZE;
  if(!frameMetaPhys[0].isFree && frameMetaPhys[0].owner == currentThread->tid)
  {
    meta = (PHYS_MEMORY[start_index] << 16) | (PHYS_MEMORY[start_index+1] << 8) | (PHYS_MEMORY[start_index+2]);
    isFree = (meta >> 23) & 0x1;
    blockSize = meta & 0x7fffff;
    ////printf("We are on start_index %i which has a block of size %i and it is free? %i\n", start_index, blockSize, isFree);
    fflush(stdout);
    start_index += sizeof(char) * 3 + blockSize;
  }
}

>>>>>>> why god why
