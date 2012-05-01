/*********************** wait.c file ***********************************/

int grave()   /* this grave() does NOT free the ZOMBIE proc */
{
  printf("\n*************************************\n"); 
  printf("Process %d %c %s", running->pid, 7,gasp[running->pid % 4]);
  printf("\n*************************************\n");
  
  tswitch();
}

int wait(exitValue) int *exitValue;
{
  PROC *p;
  int  i, found;

  while(1){
     found = 0;
     for (i=0; i<NPROC; i++){
          p = &proc[i];
          if (p->ppid == running->pid && p->status != FREE){ 
             found = 1;
             /* lay the dead child to rest */
             if (p->status == ZOMBIE){
                *exitValue = p->exitValue;
                 p->status = FREE;       /* free its PROC */
                 putproc(p);
                 nproc--;
                 return(p->pid);         /* return its pid */
             }
          }
     }
     if (!found)                         /* no child */
          return(-1);
     sleep(running);                     /* has kids still alive */
  }
}

int enterSleep(p) PROC *p;
{
  PROC *q;
  if (sleepList == 0)
    sleepList = p;
  else{
    q = sleepList;
    while (q->next)
       q = q->next;
    q->next = p;
  }
  p->next = 0;
}

int sleep(event) int event;
{
  printf("\n*************************************\n");
  printf("Proc %d going to sleep on event=%x", running->pid, event);
  printf("\n*************************************\n");
  running->status = SLEEP;
  running->event = event;
  // enter sleepList FIFO
  enterSleep(running);
  tswitch();
}
  

/* wake up ALL tasks sleeping on event */
int wakeup(event) int event;
{
  PROC *p, *q; int i;

  p = q = sleepList;

  while(p){
    if (p->event != event){
      q = p;
      p = p->next;
      continue;
    }
 
    // p->event==event
    if (p==sleepList){
	 sleepList = p->next;
         p->status = READY;
         p->event = 0;
         enqueue(&readyQueue, p);
         printf("wakeup proc %d\n", p->pid);
         p = q = sleepList;
         continue;
    }
    // not the first element   
    q->next = p->next;   // delete p from list
    p->event = 0;
    p->status = READY;
    enqueue(&readyQueue, p);
    printf("wakeup proc %d\n", p->pid);
    p = q->next;
  }          
}


// YOU WRITE C CODE TO DO THESE

int do_exit(v) int v;
{
  int i; PROC *p;
  printf("proc %d in kexit(): exitValue=%d\n", running->pid, v);

  (1). if caller is P1 && there are other procs still existing, do NOT die;

  (2). Send children (dead or alive) to P1;
       Wakeup P1 if any child is a ZOMBIE;

  (3). Record exitValue and become a ZOMBIE;

  (4). wakeup parent;

  (5). call grave() to tswitch();
}


int do_wait(uptr) int *uptr; // 0 for Kmode, nonzero for umode
{
  wait for a ZOMBIE child:
       write ZOMBIE's exitValue to U space;
       return ZOMBIE's pid to U space;

}





