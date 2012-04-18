#include "my_types.h"

/*                         I/O Functions.                     */

u16 get_word(segment, offset) u16 segment; u16 offset;
{
	char left  = get_byte(segment, offset);
	char right = get_byte(segment, offset + 1);

  u16 merge = ((left << 8) & 0xF0) & right;

	return merge;
}

void put_word(word, segment, offset) u16 word; u16 segment; u16 offset;
{
  char left  = (word >> 8) & 0x0F;
  char right = word & 0x0F;

  put_byte(left , segment, offset);
  put_byte(right, segment, offset + 1);
}

/*                       String I/O Functions                 */

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
int BASE = 10;

/*
   The function rpu(x) recursively generates the digits of x % 10 and 
   prints the digits on return. 

   With printu(x), it is trivial to print signed integers.
*/

int printu(x) u16 x;
{
  BASE = 10;
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
    c = table[x % BASE];
    rpu(x / BASE);
    putc(c);
  }
}

int printx(x) u16 x;
{
  BASE = 16;
  rpu((u32)x);
}

int printl(x) u32 x;
{
  BASE = 10;
  if (x==0)
    putc('0');
  else
    rpl((u32)x);
  putc(' ');
}

int rpl(x) u32 x;
{
  u8 c;
  if (x){
    c = table[x % BASE];
    rpl( (u32)(x / BASE) );
    putc(c);
  }
}

int printX(x) u32 x;
{
  BASE = 16;
  rpl((u32)x);
}

int printi(x) int x;
{
  if (x<0) {
    putc('-');
    x = -x;
  }
    printu(x);
}

   /*
   By changing the divide-mod base to 16, we can implement printx(x) to print
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


int printf(fmt) char *fmt;
{
  char *cp = fmt;              // cp points at the fmt string
  u16  *ip = (int *)&fmt + 1;  // ip points at first item to be printed on stack
  u32  *up;
  while (*cp){
    if (*cp != '%'){
        putc(*cp);
        if (*cp=='\n')
          putc('\r');
        cp++;
        continue;
    }
    cp++;
    switch(*cp){
      case 'c' : putc  (*ip); break;
      case 's' : prints(*ip); break;
      case 'u' : printu(*ip); break;
      case 'd' : printi(*ip); break;
      case 'x' : printx(*ip); break;
      case 'l' : printl(*(u32 *)ip++); break; 
      case 'X' : printX(*(u32 *)ip++); break; 
    }
    cp++; ip++;
  }
}