/*
                       I/O Functions.

   Based on getc() and putc() of BIOS, we can implement the following simple
   terminal I/O functions for MTX.

1.1. 
*/

#include "my_types.h"

u16 get_word(segment, offset) u16 segment; u16 offset;
{
	putc('G');
	putc('W');
	putc('\n');

	// get_byte();
	// get_byte();

	return 1;
}

int put_word(word, segment, offset) u16 word; u16 segment; u16 offset;
{
	putc('P');
	putc('W');
	putc('\n');

	// put_byte();
	// put_byte();

	return 1;
}

//                       String I/O Functions

/* gets(char s[ ]) : inputs a string from the keyboard, where s[ ] is a 
     memory area with enough space for the incoming string.  */

int gets(s) char s[ ];  
{
	while( (*s = getc()) != '\r'){
        putc(*s++);
    }
    *s = 0;
}

int prints(s) char *s;
{
	while (*s)
    putc(*s++);
}

char *table = "0123456789ABCDEF";

/*
   The function rpu(x) recursively generates the digits of x % 10 and 
   prints the digits on return. 

   With printu(x), it is trivial to print signed integers.
*/

int printu(x) u16 x;
{
	if (x==0)
       putc('0');
    else
       rpu(x);
    putc(' ');
} 

int rpu(x) u16 x;
{
    u8 c;
    if (x){
    c = table[x % 10];
        rpu(x / 10);
    }
    putc(c);
}

int printd(x) int x;
{
    if (x<0){
        putc('-');
        x = -x;
    }
    printu(x);
}

   /*
   By changing the divde-mod base to 16, we can implement printx(x) to print
   in HEX. Similarly, by changing the type to u32, we can print long values,
   etc.
   */

   /*
1.3. Assume that we have printc(), which is just putc(), prints(), printd(),
     printu(), printx(), printo() and printl() (long). It is easy to implement 
     a printf(char *fmt, args) function for formatted printing, where fmt is 
     a usual format string containing conversion symbols %c, %s, %d, %u, %x,
     %o, %l, etc. Instead of showing the actual code, it suffices to explain 
     the algorithm of printf().
     */
   
printf(fmt) char *fmt;  // arg list should follow fmt on stack  
{                                 
      // upon entry, stack contains :| bp | PC | fmt | args .....=>(HIGH)
      //                               bp 
      /* TODOFAOSF
    u16  *bp = getbp();   // getbp() returns current stack frame pointer bp
    char *cp = *(bp + 2); // cp = fmt
    u16  *ip =  (bp + 3); // ip point at first arg
      */
      // use cp to scan fmt, output each char until %X, where X=c|s|d|u|x|o|l.
      // call printX(*(Xtype *)ip) to print the current arg by X type;
      // advance ip to point at the next arg on stack, etc.
} 