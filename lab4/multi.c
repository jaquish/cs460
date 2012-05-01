typedef unsigned char   u8;
typedef unsigned short u16;
typedef unsigned long  u32;

/*********** MTX Multitasking System ************/
#define NULL      0
#define NPROC     9
#define SSIZE  1024
   
#define FREE      0     // proc statuc
#define READY     1 
#define SLEEP     2
#define ZOMBIE    3

typedef struct proc{
    struct proc *next;
    int  *ksp;           
 
           int   uss, usp;  // at offsets 4, 6

           int   pid;       /* pid = 0 to NPROC-1 */
           int   status; 
           int   pri;      /* scheduling priority */
           int   ppid;     /* parent pid */
           struct proc *parent;

           int    event;
           int    exitValue;
           char name[32];

           int   kstack[SSIZE];   // Kmode per process stack
}PROC;

PROC proc[NPROC], *running, *freeList, *readyQueue, *sleepList;

int procsize = sizeof(PROC);
int nproc, color;

int body();
char *pname[]={"Sun", "Mercury", "Venus", "Earth",  "Mars", "Jupiter", 
	       "Saturn", "Uranus", "Neptune" };

#include "queue.c"
#include "wait.c"
#include "int.c"


int initialize()
{
  int i; PROC *p;

  printf("MTX initializing ....\n");
   
  for (i=0; i < NPROC; i++){
      proc[i].pid = i; 
      proc[i].status = FREE;
      proc[i].next = (PROC *)&proc[(i+1)];

      strcpy(proc[i].name, pname[i]);
  }
  proc[NPROC-1].next = NULL;
  freeList = &proc[0];         // all PROC are FREE initially

  readyQueue = 0;
  sleepList = 0;

  // create P0
  p = getproc();              // get a FREE PROC

  p->status = READY;
  p->pri = 0;                /* lowest priority  0 */
  p->pid = 0;                /* process 0 or P0 */
  running = p;
  p->ppid = running->pid;    /* P0's parent is P0 */
  nproc = 1;

  printf("initialization complete\n"); 
}


/***********************************************************
  kfork() creates a child proc and returns the child pid.
  When scheduled to run, the child process resumes to body();
************************************************************/
int kfork()
{
  PROC *p;
  int  i, child;
  u16  segment;

  /*** get a PROC for child process: ***/
  if ( (p = getproc()) == NULL){
       printf("no more proc\n");
       return(-1);
  }

  /* initialize the new proc and its stack */
  p->status = READY;
  p->ppid = running->pid;
  p->parent = running;
  p->pri  = 1;                 // all of the same priority 1


  // clear all SAVed registers on stack
  for (i=1; i<10; i++)
      p->kstack[SSIZE-i] = 0;
 
  // fill in resume address
  p->kstack[SSIZE-1] = (int)body;

  // save stack TOP address in PROC
  p->ksp = &(p->kstack[SSIZE - 9]);

  enqueue(&readyQueue, p);
  nproc++;


  /************** YOU WRITE C CODE TO DO THESE *****************
  1. compute new proc's segment by pid:
        pid=1 ==> 0x2000; 2 ==> 0x3000; 3 ==> 0x4000, etc.
  2. load /u1 to segment

  3. Initialize new proc's ustack AS IF it did INT 80 from its virtual
     address 0

  4. newProc.uss = segment; newProc.usp = (a value)
  
  printf("Proc %d forked a child %d at segment=%x\n",
          running->pid, p->pid, segment);
  return(p->pid);
}


int do_switch()
{ 
   printf("P%d switch process\n", running->pid);
   tswitch();
}

int do_kfork()
{
   int new;
   new = kfork();
   if (new < 0)
       printf("kfork failed\n");
   else
       printf("P%d return from kfork() : child = %d\n",
               running->pid, new);
}

/*****************************************
   All proc share the same body function,
   as if called by the process itself.
******************************************/
 
int body()
{
  char c;
  while(1){
    printf("------------------------------------------\n"); 
    printf("I am process P%d    My parent=%d\n", running->pid, running->ppid);
    
    color = 0x000A + running->pid;
    printf("******************************************\n");
    printf("freeList   = "); printList(freeList);
    printf("readyQueue = "); printList(readyQueue);
    printf("sleepList  = "); printList(sleepList);
    printf("******************************************\n");

    printf("input a command : [s|q|f|w|u] : ");
    c = getc();   
    printf("%c\n", c);

    switch(c){
      case  's' : do_switch();  break;
      case  'q' : do_exit(100); break;   /* no return */
      case  'f' : do_kfork();   break;
      case  'w' : do_wait(0);   break;
      case  'u' : goUmode();    break;

      default   :              break;  
    }
  }
}

int int80h();

int set_vec(vector, addr) u16 vector, addr;
{
    u16 location,cs;
    location = vector << 2;
    put_word(addr, 0, location);
    put_word(0x1000,0,location+2);
}



//*************** main() ***************
main()
{
   color = 0x000A;
   printf("\nWelcome to the MTX Operating System\n");
     initialize();
 
     /********** WRITE YOUR C CODE TO **************
         initialize vector 80 : (CS,PC)=(0x1000, _inth)
     ***********************************************/

   printf("P0 forks P1\n");
     kfork();
   printf("P0 switches to P1\n");
     tswitch();
   printf("P0 resumes: all dead, happy ending!\n");
}

//******** scheduler *******************

int scheduler()
{ 
  if (running->status == READY)
    enqueue(&readyQueue, running);

  running = dequeue(&readyQueue);
}


