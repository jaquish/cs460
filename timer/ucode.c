// ucode.c file

char *cmd[]={"getpid", "ps", "chname", "kmode", "switch", "wait", "die", 
             "fork", "exec", "color", 0};

int show_menu()
{
   printf("************************* Menu *****************************\n");
   printf("*  ps  chname  kmode  switch  wait  die  fork  exec  color *\n");
   /*         1     2      3       4      5     6    7     8      9   */
   printf("************************************************************\n");
}

int find_cmd(name) char *name;
{
   int i;   char *p;
   i = 0;   p = cmd[0];
   while (p){
         if (strcmp(p, name)==0)
            return i;
         i++;  p = cmd[i];
   } 
   return(-1);
}


int getpid()
{
   return syscall(0,0,0);
}

int ps()
{
   syscall(1, 0, 0);
}

int chname()
{
    char s[64];
    printf("\ninput new name : ");
    gets(s);
    syscall(2, s, 0);
}

int kmode()
{
    printf("kmode : enter Kmode via INT 80\n");
    printf("proc %d going K mode ....\n", getpid());
        syscall(3, 0, 0);
    printf("proc %d back from Kernel\n", getpid());
}    

int kswitch()
{
    printf("proc %d enter Kernel to switch proc\n", getpid());
        syscall(4,0,0);
    printf("proc %d back from Kernel\n", getpid());
}

int uwait()
{
    int child, exitValue;
    printf("proc %d enter Kernel to wait for a child to die\n", getpid());
    child = syscall(5, &exitValue, 0);
    printf("proc %d back from wait, dead child=%d", getpid(), child);
    if (child>=0)
        printf("exitValue=%d", exitValue);
    printf("\n"); 
} 

int die()
{
   char exitValue;
   printf("\nenter an exitValue (0-9) : ");
   exitValue=getc() - '0';
   printf("enter kernel to die with exitValue=%d\n");
        syscall(6,exitValue,0);
}

int exit(v) int v;
{
  syscall(99, v, 0);
}

int ufork()
{
  int child;
  child = syscall(7,0,0,0);
  if (child)
    printf("parent % return form fork, child=%d\n", getpid(), child);
  else
    printf("child %d return from fork, child=%d\n", getpid(), child);
}

int uexec()
{
  int r;
  char filename[32];
  printf("\nenter exec filename : ");
  gets(filename);
  r = syscall(8,filename,0,0);
  printf("exec failed\n");
}

int chcolor()
{
  int y;
  printf("\ninput color [r|g|c|y] ; ");
  y = getc();
  syscall(9,y,0);
}

int putc(c) char c;
{
  return syscall(10, c, 0, 0);
}

int invalid(name) char *name;
{
    printf("Invalid command : %s\n", name);
}

