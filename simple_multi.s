.globl begtext, begdata, begbss                      ! needed by linker
.globl _main, _getc, _putc
.globl _tswitch, _running, _scheduler

.text                                                ! these tell as:	
begtext:                                             ! text,data,bss segments
.data                                                ! are all the same.
begdata:
.bss
begbss:
.text           

start:
        mov     ax,cs                   ! establish segments 
        mov     ds,ax                   ! we know ES,CS=0x1000. Let DS=CS  
        mov     ss,ax                   ! SS = CS ===> all point to 0x1000
        mov     es,ax
        mov     sp,#0                   ! SP = 64KB

        call _main                      ! call main[] in C

dead:	jmp dead

_getc:
        xorb   ah,ah 
        int    0x16  
        ret 

_putc:           
        push   bp
        mov    bp,sp
        movb   al,4[bp]
        movb   ah,#14  
        mov    bx,#0x000B 
        int    0x10    
        pop    bp
        ret
	
_tswitch:
SAVE:   push ax
        push bx
        push cx
        push dx
        push bp
        push si
        push di
        pushf
        mov   bx, _running
        mov   2[bx], sp

FIND:   call _scheduler

RESUME: mov   bx, _running
        mov   sp, 2[bx]
        popf
        pop  di
        pop  si
        pop  bp
        pop  dx
        pop  cx
        pop  bx
        pop  ax
        ret
