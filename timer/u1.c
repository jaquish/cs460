#include "uio.c"
#include "ucode.c"

main()
{ 
  char name[64]; int pid, cmd;

  while(1){

       printf("==============================================\n");
       printf("I am proc %din U mode: running at segment=%x\n",
                getpid(), getcs());

       #include "ubody.c"
  }
}

