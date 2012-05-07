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

int goUmode();

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

  {
    // compute new proc's segment by pid:
    u16 segment = (p->pid + 1) * 0x1000;
    u16 seg_size = 0x1000;

    // load /bin/u1 to segment
    load("bin/u1", segment);

    printf("loaded to segment %u\n", segment);

    // Initialize new proc's ustack AS IF it did INT 80 from its virtual address 0
    {
      int i;
      for (i = 1; i < 13; ++i)
      {
        u16 word;
        switch(i) {
          case 1:   word = 0x0200;  break;  // uFlag

          case 2:
          case 11:
          case 12:  word = segment; break;  // uCS, uES, uDS

          default:  word = 0;       break;  // pretty much everything else, including PC
        }

        put_word(word, segment, seg_size-i*2);  // stack starts at highest end of segment
      }
    }

    p->uss = segment;
    p->usp = seg_size - 12*2; // usp is byte address, x2

  }
  
  printf("Proc %d forked a child %d at segment=%x\n", running->pid, p->pid, segment);
  
  return(p->pid);
}

int copy_image(segment) u16 segment;
{
  int from_segment = (running->pid + 1) * 0x1000;
  int i;
  for(i = 0; i < 32768; i++) {                        // dumb copy_image doesn't know the size, do all 64KB
    put_word(get_word(from_segment, i), segment, i);
  }
}

int ufork()
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
  p->pri  = running->pri;                 // all of the same priority 1


  // Kmode stack is still wiped
  for (i=1; i<10; i++)
      p->kstack[SSIZE-i] = 0;
 
  // fill in resume address
  p->kstack[SSIZE-1] = (int)goUmode;  // jump to Umode

  // save stack TOP address in PROC
  p->ksp = &(p->kstack[SSIZE - 9]);

  enqueue(&readyQueue, p);
  nproc++;

  {
    // compute new proc's segment by pid:
    u16 segment = (p->pid + 1) * 0x1000;
    u16 seg_size = 0x1000;

    // load to segment
    copy_image(segment);

    printf("loaded to segment %u\n", segment);

    p->uss = segment;
    p->usp = running->usp;  // resumes from same point

    // modify return value as if it executed pid=fork() with return value of 0
    put_word(0, segment, running->usp + 8*2);
  }
  
  printf("Proc %d forked a child %d at segment=%x\n", running->pid, p->pid, segment);
  
  return(p->pid);
}

int exec(filepath) char* filepath;
{
  // copy name into buffer
  char path[128];
  int i;

  u16 segment =  (running->pid + 1) * 0x1000;
  u16 seg_size = 0x1000;

  for (i = 0; i < 128; ++i)
  {
    path[i] = get_byte(segment, filepath + i);
    if (path[i] == '\0')
      break;
  }

  load(path, segment);

  // fix ustack to execute from beginning of new image
  {
    int i;
    for (i = 1; i < 13; ++i)
    {
      u16 word;
      switch(i) {
        case 1:   word = 0x0200;  break;  // uFlag

        case 2:
        case 11:
        case 12:  word = segment; break;  // uCS, uES, uDS

        default:  word = 0;       break;  // pretty much everything else
      }

      put_word(word, segment, seg_size-i*2);  // stack starts at highest end of segment
    }
  }

  return;
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

  // initialize vector 80 : (CS,PC)=(0x1000, _inth)
  put_word(int80h, 0x0000, 80*4);
  put_word(0x1000, 0x0000, 80*4 + 2);

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

// print status of all processes
int do_ps()
{
  int i;
  printf("***** Process Status ***** \n");

  for (i = 0; i < NPROC; ++i)
  {
    PROC* p = &proc[i];
    printf("pid=%d ppid=%d status=", p->pid, p->ppid);
    switch(p->status) {
      // case RUNNING: printf("RUNNING");             break;
      case READY:   printf("READY");               break;
      case SLEEP:   printf("SLEEP(%d)", p->event); break;
      case ZOMBIE:  printf("ZOMBIE");              break;
      case FREE:    printf("FREE");                break;
      default:      printf("ERROR");               break;
    }
    printf(" name=%s", p->name);
    printf("\n");
  }
  printf("\n");
  return 1;
}

// change name of process
int chname(b) char* b;
{
  int i;
  printf("new name:");

  for (i = 0; i < 32; ++i)  // size of name
  {
    running->name[i] = get_byte(running->uss, b+i);
    if (running->name[i] == '\0')
      break;
    else 
      printf("%c", running->name[i]);
  }
  printf("\n");

  if (running->name[31] != '\0')
    running->name[31] == '\0';
}

int kmode()
{
  body();
}


