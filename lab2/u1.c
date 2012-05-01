#include "io.c"   // YOUR gets(), printf(), etc.

main()
{
  char name[64];
  u16 segment;

  printf("\nThis is u1 running\n");
  while(1){
    printf("what's your name? : ");
    gets(name);
    if (strcmp(name, "quit")==0){
       printf("quit : go back to OS at 0x1000\n"); 
       return;
    }
    printf("Welcome %s\n", name);

  }
}
