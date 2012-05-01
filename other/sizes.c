//
//  main.c
//  sizeof
//
//  Created by Jonathan Field on 07/10/2011.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include <stdio.h>


int a = sizeof(char);
 
int b = sizeof(short);
  
int c = sizeof(int);
  
int d = sizeof(long);
    
int e = sizeof(float);
    
int f = sizeof(double);
   
int g = sizeof(long double);
   


int main (int argc, const char * argv[])
{

    
    printf("Char is %d bytes\n",a);
    printf("Short is %d bytes\n",b);
    printf("Integer is %d bytes\n",c);
    printf("Long is %d bytes\n",d);
    printf("Float is %d bytes\n",e);
    printf("Double is %d bytes\n",f);
    printf("Long Double is %d bytes\n",g);
    
    return 0;
    
}

