/****************** int.c file ***********************/

IMPLEMENT YOUR 
    do_ps(), chname(), do_wait(), do_exit()
in MTX Kernel

int kcinth()
{
   int  a,b,c,d;

   // get a,b,c,d from ustack
 
   switch(a){
       case 0 : r = running->pid;     break;
       case 1 : r = do_ps();          break;
       case 2 : r = chname(b);        break;
       case 3 : r = kmode();          break;
       case 4 : r = tswitch();        break;
       case 5 : r = do_wait(b);       break;
       case 6 : do_exit(b);           break;       
 
       default: printf("invalid syscall # : %d\n", a);
                break;
   }

   // put return value r into saved AX for return to Umode;
}
