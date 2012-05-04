.globl begtext, begdata, begbss                      ! needed by linker
.globl _getc,_putc,_getcs
.globl _main, _syscall

.text                                                ! these tell as:	
begtext:                                             ! text,data,bss segments
.data                                                ! are all the same.
begdata:
.bss
begbss:
.text               

        call _main

dead:   jmp   dead

_getcs:
        mov   ax, cs
        ret

        !---------------------------------------------
        !  char getc[]   function: returns a char
        !---------------------------------------------
_getc:
        xorb   ah,ah           ! clear ah
        int    0x16            ! call BIOS to get a char in AX
        ret 

        !----------------------------------------------
        ! void putc[char c]  function: print a char
        !----------------------------------------------
_putc:           
        push   bp
        mov    bp,sp

        movb   al,4[bp]        ! get the char into aL
        movb   ah,#14          ! aH = 14
        mov    bx,#0x000E           ! bL = display page#
        int    0x10            ! call BIOS to display the char

        pop    bp
        ret

_syscall:
        int    80
        ret

