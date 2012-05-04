.globl begtext, begdata, begbss                      ! needed by linker

.globl _getc, _putc, _getcs
.globl _main, _syscall
	
.text                                                ! these tell as:	
begtext:                                             ! text,data,bss segments
.data                                                ! are all the same.
begdata:
.bss
begbss:
.text               

        call _main
	
! if ever return, exit(0)
	push  #0
        push  #99
        call  _syscall
	
dead:   jmp   dead

_getcs:
        mov   ax, cs
        ret

_getc:
        xorb   ah,ah 
        int    0x16  
        ret

	
! putc(c) is now a syscall(10,c) to MTX kernel
! which uses putc(c) in VID driver to display the char
!_putc:           
        push   bp
        mov    bp,sp
        movb   al,4[bp]
        movb   ah,#14  
        mov    bx,#0x000B
        int    0x10    
        pop    bp
        ret


	
_syscall:
        int    80
        ret

