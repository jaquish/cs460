#include "print.c"  /* <===== use YOUR OWN io.c with printf() ****/

#define NPROC     9        
#define SSIZE  1024                /* kstack int size */

#define DEAD      0                /* proc status     */
#define READY     1      
#define FREE      2

#define NULL 0

typedef struct proc{
    struct proc *next;   
           int ksp;               /* saved sp; offset = 2 */
           int pid;
           int p_pid;             /* pid of parent process */
           int status;            /* READY|DEAD, etc */
           int priority;          /* from 0 -> intMAX */
           int kstack[SSIZE];     // kmode stack of task
}PROC;

PROC proc[NPROC];
PROC* running;
PROC* freeList;
PROC* readyQueue;

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
  printf("FreeList:");       printQueue(freeList);   printf("\n");
}

// enqueue based on priority (if two have the same priority, the oldest process gets priority)
void enqueue(new_proc, queue) PROC* new_proc; PROC** queue;
{
  printf("insert process %d with priority: %d \n", new_proc->pid, new_proc->priority);

  // handle edge cases, 1) empty queue and 2) higher priority than first element
  if (*queue == NULL || new_proc->priority > (*queue)->priority) {
    PROC* old_head = *queue;

        printf("EDGE CASE\n");


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

int grave(){
  printf("\n*****************************************\n"); 
  printf("Task %d %s\n", running->pid,gasp[(running->pid) % 4]);
  printf("*****************************************\n");
  running->status = DEAD;

  tswitch();   /* journey of no return */        
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

int body()
{  char c;
   while(1){
      ps();
      printQ();

      printf("I am Proc %d in body()\n", running->pid);
      printf("Input a char : [s|q|f] \n");
       c=getc();
       switch(c){
            case 's': tswitch(); break;
            case 'q': grave();   break;
            case 'f': kfork();   break;
            default :            break;  
       }
   }
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
