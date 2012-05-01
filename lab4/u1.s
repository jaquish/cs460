.globl begtext, begdata, begbss                      ! needed by linker

.text                                                ! these tell as:	
begtext:                                             ! text,data,bss segments
.data                                                ! are all the same.
begdata:
.bss
begbss:
.text               
.globl  _main, _syscall, _exit
.globl  _getcs, _color

        call _main
	
! if ever return, exit(0)
	push  #0
        call  _exit

_getcs:
        mov   ax, cs
        ret

_syscall:
        int    80
        ret
