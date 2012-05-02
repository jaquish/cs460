/******** getproc()/putproc() ***********/
PROC *getproc()
{
 PROC *p;
 p = freeList;
 if (p) 
    freeList = freeList->next;
 return(p);
}

putproc(p) PROC *p;
{
  p->next = freeList;
  freeList = p;   
}

enqueue(queue, ptr) PROC **queue, *ptr;
{

   PROC *p = *queue;

   if (p == NULL || ptr->pri > p->pri){
         *queue = ptr;
          ptr->next = p;
   }
   else{
         while (p->next && ptr->pri <= p->next->pri)
                p = p->next;
         ptr->next = p->next;
         p->next = ptr;
   }

}

PROC *dequeue(queue) PROC **queue;
{
  PROC *p = *queue;
  if (p)
     *queue = (*queue)->next;
  return p;
}

printQueue(queue) PROC *queue;
{
   PROC *p = queue;
   printf("----------------------------\n");
   printf("freeList   = ");
   printList(freeList);

   printf("readyQueue = ");
   while(p){
        printi(p->pid); prints(" --> ");
        p = p->next;
   }
   printf("\n");
   printList(sleepList);
   printf("----------------------------\n");
}

int printList(p) PROC *p;
{
   while(p){
       printf("%d ==> ", p->pid);
       p = p->next;
   }
   printf("NULL\n");
}

int printsleep()
{
  PROC *p = sleepList;
  printf("sleepList = [proc,event]=");
  while(p){
    printf("[P%d, %d] ==> ", p->pid, p->event);
    p = p->next;
  }
  printf("NULL\n");
}


char *gasp[NPROC]={
  "Oh! I'm dying ....\n", 
  "Oh! You are killing me ....\n",
  "Oh! I'm a goner ....\n", 
  "Oh! I'm mortally wounded ....\n",
};
