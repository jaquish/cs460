.globl begtext, begdata, begbss                      ! needed by linker

.globl _getc,_putc                                   ! EXPORT
.globl _main,_prints                                 ! IMPORT these 

.text                                                ! these tell as:	
begtext:                                             ! text,data,bss segments
.data                                                ! are all the same.
begdata:
.bss
begbss:
.text 

start:
        mov     ax,cs                   ! set segment registers for CPU
        mov     ds,ax                   ! we know CS=segment. Let DS=SS
        mov     es,ax                   ! ES = CS
        mov     ss,ax                   ! SS = CS ===> all point at segment
        mov     sp,#0                   ! SP = 64KB

        call _main                      ! call main() in C

gomtx:	jmpi   0, 0x1000                ! jmp back to SOS at 0x1000
	
		
        !---------------------------------------------
        !  char getc()   function: returns a char
        !---------------------------------------------
_getc:
        xorb   ah,ah           ! clear ah
        int    0x16            ! call BIOS to get a char in AX
        ret 

        !----------------------------------------------
        ! void putc(char c)  function: print a char
        !----------------------------------------------
_putc:           
        push   bp
	mov    bp,sp
	
        movb   al,4[bp]        ! get the char into aL
        movb   ah,#14          ! aH = 14
        movb   bl,#0x0C        ! bL = RED color 
        int    0x10            ! call BIOS to display the char

        pop    bp
	ret
