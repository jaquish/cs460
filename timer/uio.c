typedef unsigned short ushort;
typedef unsigned long  ulong;


prints(s) char *s;
{
   while (*s){
      putc(*s);
      s++;
   }
}

gets(s) char *s;
{   char c; int len=0; 
    while ( (c=getc()) != '\r' && len < 64){
           *s++ = c; len++;
           putc(c); 
    }
    prints("\n\r");
    *s = '\0';
}

char *ctable = "0123456789ABCDEF";

int rpo(x) ushort x;
{
   char c;
   if (x==0) return;
   c = ctable[x%8];
   rpo(x/8);
   putc(c);
}

int printo(x) ushort x;
{
  if (x==0){
    putc('0'); putc(' ');
     return;
  }
  rpo(x);
  putc(' ');
}

int rpi(x) int x;
{
   char c;
   if (x==0) return;
   c = ctable[x%10];
   rpi(x/10);
   putc(c);
}
  
int printi(x) int x;
{
    if (x==0){
       prints("0 ");
       return;
    }
    if (x < 0){
       putc('-');
       x = -x;
    }
    rpi(x);
    putc(' ');
}

int rpli(x) long x;
{
   char c;
   if (x==0) return;
   c = ctable[x%10];
   rpli(x/10);
   putc(c);
}

int printli(x) ulong x;
{
    if (x==0){
       prints("0 ");
       return;
    }
    if (x < 0){
       putc('-');
       x = -x;
    }
    rpli(x);
    putc(' ');
}

int rpx(x) ushort x;
{
   char c;
   if (x==0) return;
   c = ctable[x%16];
   rpx(x/16);
   putc(c);
}

int printx(x) ushort x;
{  
  prints("0x");
   if (x==0){
      prints("0 ");
      return;
   }
   rpx(x);
  putc(' ');
}

int rplx(x) ulong x;
{
   char c;
   if (x==0) return;
   c = ctable[x%16];
   rplx((ulong)x/16);
   putc(c);
}

int printlx(x) ulong x;
{
   prints("0x");
   if (x==0){
      prints("0 ");
      return;
   }
   rplx(x);
  putc(' ');
}

pns(s, n) char *s; int n;
{ 
   while (*s && n){
      putc(*s);
      s++; n--;
   }
}

int rpl(x) ulong x;
{
  char c;
  if (x==0)
    return;
  c = ctable[x % 10];
  rpl(x / 10);
  putc(c);
}

int align(x) ulong x;
{
  int count, i;
  count = 10;
  if (x==0) 
     count = 9;
  while (x){
    count--;
    x = x /10;
  }

  while(count){
    putc(' ');
    count--;
  }
}

int printl(x) ulong x;
{
  align(x);
  if (x==0){
    putc('0'); putc(' ');
    return;
  }
  rpl(x);
  putc(' ');
}

int printf(fmt) char *fmt;
{
  char   *cp;
  ushort *ip;
  ulong  *up;
 
  cp = fmt;
  ip = (int *)&fmt + 1;

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
      case 'd' : printi(*ip); break;
      case 'x' : printx(*ip); break;
      case 'l' : up = (ulong *)ip;
                 printl(*up); ip++; break;
      case 's' : prints(*ip); break;
      case 'c' : putc(*ip);   break;
    }
    cp++; ip++;
  }
}
