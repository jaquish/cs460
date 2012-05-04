typedef unsigned short ushort;

/*********** MTX Multitasking System ************/

#define NULL      0
#define NPROC     9
#define SSIZE  1024
#define NQUEUE NPROC    // at most NPROC ready queues
   
#define FREE      0     // proc statuc
#define READY     1 
#define SLEEP     2
#define ZOMBIE    3


typedef struct proc{
    struct proc *next;
           int  *ksp; /* offset = 4 bytes */
 
           int   uss, usp;  // at 4, 6

           int   pid;       /* pid = 0 to NPROC-1 */
           int   status; 
           int   pri;      /* scheduling priority */
           int   ppid;     /* parent pid */

           struct proc *parent;
           int    event;
           char   *deathCry;
           int    exitValue;
           
           char name[32];
           int time;

           int   kstack[SSIZE];   // Kmode per process stack
}PROC;

struct rqueue{
       int priority;    // 0, 1,...NQUEUE-1
       PROC *queue;     // PROC queue of this priority level
};

extern struct rqueue rqueue[ ];

extern PROC proc[NPROC], *running, *freeList, *sleepList;
extern int nproc;

