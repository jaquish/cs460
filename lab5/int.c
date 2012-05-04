int kcinth()
{
  int  a,b,c,d, r;

  // get a,b,c,d from ustack
  u16 segment = (running->pid + 1) * 0x1000;
  a = get_word(segment, running->usp + 13*2); 
  b = get_word(segment, running->usp + 14*2);
  c = get_word(segment, running->usp + 15*2);
  d = get_word(segment, running->usp + 16*2);

  printf("kcinth() a=%d b=%d c=%d d=%d\n", a,b,c,d);

  switch(a)
  {
    case 0 : r = running->pid;     break;
    case 1 : r = do_ps();          break;
    case 2 : r = chname(b);        break;
    case 3 : r = kmode();          break;
    case 4 : r = tswitch();        break;
    case 5 : r = do_wait(b);       break;
    case 6 : r = ufork();          break;       
    case 7 : exec(b);              break;
    
    case 8 : do_exit(b);           break;

    default: printf("invalid syscall # : %d\n", a);
              break;
 }

 // put return value r into saved AX for return to Umode;
 put_word(r, segment, running->usp + 8*2);
}
