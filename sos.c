#include "ext2.h"           // EXT2 INODE, DIR struct types
#include "io.c"             // YOUR printf() with "%c %s %d %x %l\n";
#include "loader.c"         // YOUR loader.c file

char filename[64];
u16 segment;

main()
{
  printf("\nSimple OS running\n");
  while(1){
    printf("enter filename to execute : ");
    gets(filename);
    segment = 0x2000;  // choose a free segment
    if (load(filename, segment)){
      printf("jmp to segment %s\n", segment);
      go(segment);
    }
    else{
      printf("exec ERROR : file not found\n");
    }
  }
}