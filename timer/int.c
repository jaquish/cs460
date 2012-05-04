
int kcinth()
{
   ushort seg, off, r;
   int    x, y, z, w;

   seg = running->uss; off = running->usp;

   x = get_word(seg, off + 2*13);
   y = get_word(seg, off + 2*14);
   z = get_word(seg, off + 2*15);
   w = get_word(seg, off + 2*16);

   switch(x){
       case 0 : r = running->pid;     break;
       case 1 : r = do_ps();          break;
       case 2 : r = chname(y);        break;
       case 3 : r = kmode();          break;
       case 4 : r = tswitch();        break;
       case 5 : r = do_wait(y);       break;
       case 6  : r = do_exit(y);      break;       
   
       case 7  : r = ufork();         break;
       case 8 :  r = exec(y);         break;
 
       case 9 : r = chcolor(y); break;

       case 10: r = putc(y); break;

       case 99 : do_exit(y);           break;
 
       default: printf("invalid syscall # : %d\n", x);
                break;
   }
   put_word(r, seg, off + 2*8);
}
