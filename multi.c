#include "print.c"  /* <===== use YOUR OWN io.c with printf() ****/

#define NPROC     9        
#define SSIZE  1024                /* kstack int size */

#define DEAD      0                /* proc status     */
#define READY     1      
#define FREE      2
#define SLEEP     3
#define ZOMBIE    4
#define RUNNING   5

#define NULL 0

typedef struct proc{
    struct proc *next;   
           int ksp;               /* saved sp; offset = 2 */
           int pid;
           int p_pid;             /* pid of parent process */
           int status;            /* READY|DEAD, etc */
           int priority;          /* from 0 -> intMAX */
           int event;             /* for sleep/wake */
           int exitValue;         // self-explanatory
           int kstack[SSIZE];     // kmode stack of task
}PROC;

PROC proc[NPROC];
PROC* running;
PROC* freeList;
PROC* readyQueue;
PROC* sleepList;

int  procSize = sizeof(PROC);

/****************************************************************
 Initializes the proc's as shown:
        running ---> proc[0] -> proc[1];

        proc[1] to proc[N-1] form a circular list:

        proc[1] --> proc[2] ... --> proc[NPROC-1] -->
          ^                                         |
          |<---------------------------------------<-

        Each proc's kstack contains:
        retPC, ax, bx, cx, dx, bp, si, di, flag;  all 2 bytes
*****************************************************************/

int body();  

int initialize()
{
  int i, j;
  PROC *p;

  for (i=0; i < NPROC; i++)
  {
    p = &proc[i];
    p->next = &proc[i+1];
    p->pid = i;
    p->p_pid = 0;
    p->status = FREE;
    p->priority = 1;
  }

  // special setup
  running = &proc[0];
  running->priority = 0;
  running->status = READY;
  running->next = NULL;

  freeList = &proc[1];
  proc[NPROC-1].next = NULL;

  readyQueue = NULL;

  sleepList = NULL;

  printf("initialization complete\n"); 
}

char *gasp[NPROC]={
     "Oh! You are killing me .......",
     "Oh! I am dying ...............", 
     "Oh! I am a goner .............", 
     "Bye! Bye! World...............",      
};

void printQueue(queue) PROC* queue;
{
  PROC* p = queue;
  while(p != NULL) {
    printf("[%d] --> ", p->pid);
    p = p->next;
  } 
  printf("NULL\n");
}

void printQ()
{
  printf("Running: ");       printQueue(running);
  printf("Ready Queue:");    printQueue(readyQueue);
  printf("FreeList:");       printQueue(freeList);   
  printf("Sleep:");          printQueue(sleepList); printf("\n");
}

void printProcStatuses()
{
  int i;
  printf("***** Process Status ***** \n");

  for (i = 0; i < NPROC; ++i)
  {
    PROC* p = &proc[i];
    printf("pid=%d ppid=%d status=", p->pid, p->p_pid);
    switch(p->status) {
      case RUNNING: printf("RUNNING");             break;
      case READY:   printf("READY");               break;
      case SLEEP:   printf("SLEEP(%d)", p->event); break;
      case ZOMBIE:  printf("ZOMBIE");              break;
      case FREE:    printf("FREE");                break;
      case DEAD:    printf("DEAD");                break;
      default:      printf("ERROR");               break;
    }
    printf("\n");
  }
  printf("\n");
}

// enqueue based on priority (if two have the same priority, the oldest process gets priority)
void enqueue(new_proc, queue) PROC* new_proc; PROC** queue;
{
  printf("insert process %d with priority: %d \n", new_proc->pid, new_proc->priority);

  // handle edge cases, 1) empty queue and 2) higher priority than first element
  if (*queue == NULL || new_proc->priority > (*queue)->priority) {
    PROC* old_head = *queue;
    *queue = new_proc;
    new_proc->next = old_head;
    return;
  }

  {
    PROC *p1 = (*queue);
    PROC *p2 = p1->next;

    while(p2 != NULL && new_proc->priority <= p2->priority)
    {
      p1 = p1->next;
      p2 = p2->next;
    }

    // insert
    p1->next = new_proc;
    new_proc->next = p2;

    return;
  }
}

PROC* dequeue(queue) PROC** queue;
{
  PROC* p = *queue;
  printf("dequeue()\n");

  if (*queue != NULL)
    *queue = (*queue)->next;

  // p->next = NULL;
  return p;
}

void enlist(new_proc, list) PROC* new_proc; PROC** list;
{
  new_proc->next = *list;
  *list = new_proc;
}

PROC* delist (list) PROC** list;
{
  PROC* p1 = *list;
  PROC* p2;

  if (p1 == NULL)
    return NULL;

  if (p1->next == NULL)
  {
    PROC* ret = p1;
    *list = NULL;
    return ret;
  }

  while(p1->next->next != NULL)
  {
    p1 = p1->next;
  }

  {
    PROC* ret = p1->next;
    p1->next = NULL;
    return ret;
  }
}

void wakeup(event) int event;
{
  if (sleepList == NULL) {
    return;

  } else {
    PROC* p1 = sleepList;
    PROC* p2;

    // edge case: wakeup front of list
    while (p1 != NULL && p1->event == event)
    {
      p1->status = READY;
      sleepList = p1->next;
      enqueue(p1, &readyQueue);
      p1 = sleepList;
      if (sleepList == NULL)
        return;
    }

    p2 = p1->next;

    // go thru rest of list
    while(p2 != NULL)
    {
      if (p2->event == event)
      {
        p2->status = READY;
        enqueue(p2, &readyQueue);
        p2 = p2->next;
        p1->next = p2;
      } else {
        p1 = p2;
        p2 = p2->next;
      }
    }
  }
}

void sleep(event) int event;
{
  running->event = event;
  running->status = SLEEP;
  printf("sleep() for pid=%d\n", running->pid);
  
  // For fairness, put running into a FIFO sleepList so that they will wakeup in order
  {
    if (sleepList == NULL) {
      sleepList = running;
      running->next = NULL;

    } else {
      PROC* p1 = sleepList;
      while(p1->next != NULL)
      {
        p1 = p1->next;
      }
      p1->next = running;
      running->next = NULL;
    }
  }

  tswitch();
}

int wait(status) int *status;
{
  int child_count = 0;
  
  {
    int i = 0;
    for (i; i < NPROC; ++i)
    {
      if (proc[i].p_pid == running->pid)
      {
        child_count++;
      }
    }
  }

  if (child_count == 0)
     return -1; // ERROR

  while(1){
    int i;
    for (i = 0; i < NPROC; ++i)
    {
      if (proc[i].status == ZOMBIE && proc[i].p_pid == running->pid)
      {
        *status = proc[i].exitValue;
        proc[i].status = DEAD;
        return proc[i].pid;
      }   
    }

    sleep(running);    // sleep at its own &PROC
  }
}

int ps()
{
  PROC *p;

  printf("running = %d\n", running->pid);

  p = running;
  p = p->next;
  printf("readyProcs = ");

  while(p != running && p->status==READY){
    printf("%d -> ", p->pid);
    p = p->next;
  }
  printf("\n");
}

/* my function */

PROC* getproc()
{
  if (freeList == NULL){
    return NULL;
  } else {
    return dequeue(&freeList);
  }
}

/*
Algorithm of kfork():
================================================================
PROC *p = getproc();         // get a FREE proc from freeList
  if (p==0) return -1;  // no more PROCs in freeList
--------------------------------------------------------
initialize p's ppid
initialize p's kstack[] AS IF it called tswitch() before
enter p into readyQueue (by priority)
return p->pid
=================================================================*/
int kfork()
{
  PROC *p = getproc();
  if (p == NULL) {
    printf("ERROR: freeList is empty\n");
    return -1;  // fail, freeList is empty
  }
    
  {
    int j;

    p->p_pid = running->pid;
    p->status = READY;

    // setup stack
    for (j=1; j<10; j++) {
      p->kstack[SSIZE - j] = 0;          // all saved registers = 0

      p->kstack[SSIZE-1]=(int)body;      // called tswitch() from body
      p->ksp = &(p->kstack[SSIZE-9]);    // ksp -> kstack top
    }

    enqueue(p, &readyQueue);
  
    return p->pid;
  }
}

void kexit()
{
  u16 exitValue;
  printf("Enter an exit value:");
  exitValue = getc();
  // - '0';
  running->exitValue = exitValue;
  printf("%u\n", exitValue);

  running->status = ZOMBIE;

  {
    int i;
    for (i = 0; i < NPROC; ++i)
    {
      if (proc[i].p_pid == running->pid) {
        proc[i].p_pid = proc[1].pid;  // give children to proc1
      }
    }
  }

  printf("\n*****************************************\n"); 
  printf("Task %d %s\n", running->pid,gasp[(running->pid) % 4]);
  printf("*****************************************\n");
  
  wakeup(&proc[running->p_pid]);
  tswitch();
}

int body()
{  char c;
   while(1){
      int status, pid;
      ps();
      printQ();

      printf("I am Proc %d in body()\n", running->pid);
      printf("Input a char : [s|q|f|p|w] \n");
       c=getc();
       switch(c){
            case 's': tswitch();           break;
            case 'q': kexit();             break;
            case 'f': kfork();             break;
            case 'p': printProcStatuses(); break;
            case 'w': pid = wait(&status);
                      if (pid < 1)
                        printf("ERROR - no children\n");
                      else
                        printf("dead child pid=%d exitvalue=%d\n", pid, status);
                      break;
            default :                      break;  
       }
   }
}

main()
{
  printf("\nWelcome to the 460 Multitasking System\n");
  initialize();
  kfork();
  printf("P0 switch to P1\n");
  tswitch();
  printf("P0 resumes: all dead, happy ending\n");
}


int scheduler()
{
  if (running->status == READY)
    enqueue(running, &readyQueue);

  running = dequeue(&readyQueue);

  printf("\n-----------------------------\n");
  printf("next running proc = %d\n", running->pid);
  printf("-----------------------------\n");
}
