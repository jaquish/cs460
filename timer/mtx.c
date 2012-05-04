/*********** MTX Multitasking System ************/

/***********************************************************
#define NULL      0
#define NPROC     9
#define SSIZE  1024
#define NQUEUE NPROC    // at most NPROC ready queues
   
#define FREE      0    // proc statuc
#define READY     1 
#define SLEEP     2
#define ZOMBIE    3
#define KILLED    4

// For KUmode: PROC added uss,usp at offsets 4,6.
//             Then, name[32], exitValue.

typedef struct proc{
    struct proc *next;
           int  *ksp;
 
           int   uss, usp;  // at 4, 6

           int   pid;      
           int   status; 
           int   pri;      
           int   ppid;     

           struct proc *parent;
           int    event;
           char   *deathCry;

           int   exitValue;
           char  name[32];

           int   time;            // for Umode running time slice
           int   killed;
           int   kstack[SSIZE];   // Kmode per process stack
}PROC;

**************************************************************/
#include "type.h"

struct rqueue rqueue[NQUEUE];       // at most NPROC separate ready queues

PROC proc[NPROC], *running, *freeList, *sleepList;

int procsize = sizeof(PROC);
int nproc, color;

int inkmode = 1;        // ADD KUmode transition flag; start in Kmode
 
int body();
char *pname[]={"Sun", "Mercury", "Venus", "Earth",  "Mars", "Jupiter", 
               "Saturn", "Uranus", "Neptune" };


/**********************************************************
#include "io.c"        
#include "queue.c"
#include "wait.c"
#include "loader.c"
#include "forkexec.c"
*********************************************************/

#include "int.c"
#include "vid.c" 
#include "timer.c"

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
   
  for (i=0; i<NQUEUE; i++){    // initialize the scheduling queues
      rqueue[i].priority = i;
      rqueue[i].queue = 0;
  }
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

 
int body()
{
  char c;
  while(1){
    printf("------------------------------------------\n"); 
    printf("I am process P%d    My parent=%d\n", running->pid, running->ppid);
    
    //color = 0x0001 + running->pid;
    printf("******************************************\n");
    printf("freeList   = "); printList(freeList);
    printQ();
    printsleep();
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

int set_vec(vector, addr) ushort vector, addr;
{
    ushort location,cs;
    location = vector << 2;
    put_word(addr, 0, location);
    put_word(0x1000,0,location+2);
}

int tinth();

//*************** main() ***************
main()
{

   vid_init();
   printf("vid_init : console display driver initialized\n");

   printf("\nWelcome to the MTX Operating System\n");
    
    initialize();
 
     set_vec(80, int80h);

   printf("P0 forks P1\n");
     kfork();

     lock();
       set_vec( 8, tinth);
       timer_init(); 

   printf("P0 switches to P1\n");

   while(1){
      if (rqueue[1].queue)
         tswitch();
   }
   printf("P0 resumes: all dead, happy ending!\n");
}

//******** scheduler *******************

int scheduler()
{ 
  PROC *p;
  int i;

  if (running->status == READY)
      enqueue(running);

  for (i=NQUEUE-1; i>=0; i--){
       running = dequeue(&rqueue[i].queue);
       if (running)
          break;
  }

  running->time = 5;
  color = 0x09 + running->pid;
  printf("next running=%d  time : ", running->pid);
}


