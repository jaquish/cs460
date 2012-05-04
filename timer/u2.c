#include "uio.c"
#include "ucode.c"

main()
{ 
  char name[64]; 
  int pid, cmd;


  while(1){

       printf("==============================================\n");
       printf("\nIch bin Prozess %d in der U Weise: das laufen im Segment=%x\n",
                getpid(), getcs());

       #include "ubody.c"
  }
}



