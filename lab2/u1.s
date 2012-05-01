.globl begtext, begdata, begbss                      ! needed by linker

.globl _getc,_putc, _diskr                           ! EXPORT
.globl _get_byte,_put_byte                           ! EXPORT (needed but unused)

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


! NEEDED BUT UNUSED - functions in io.c depend on them, we just need printf from io.c
!*===========================================================================*
!*                              get_byte                                     *
!*===========================================================================*
! c = get_byte(segment, offset)
_get_byte:
        push bp                 ! save bp
        mov bp,sp               ! stack frame pointer
        push es                 ! save es

        mov es,4[bp]            ! load es with segment value
        mov bx,6[bp]            ! load bx with offset from segment
        seg es                  ! go get the byte
        movb al,[bx]            ! al = byte
        xorb ah,ah              ! ax = byte

        pop es                  ! restore es
        pop bp                  ! restore bp
        ret                     ! return to caller

!*===========================================================================*
!*                              put_byte                                     *
!*===========================================================================*
! put_byte(char,segment,offset)
_put_byte:
        push bp                 ! save bp
        mov  bp,sp              ! stack frame pointer
        push es                 ! save es
        push bx

        mov  es,6[bp]           ! load es with seg value
        mov  bx,8[bp]           ! load bx with offset from segment
        movb al,4[bp]           ! load byte in aL
        seg  es                 ! go put the byte to [ES, BX]
        movb [bx],al            ! there it goes

        pop  bx                 ! restore bx
        pop  es                 ! restore es
        pop  bp                 ! restore bp
        ret                     ! return to caller

       !---------------------------------------
       ! diskr(cyl, head, sector, buf)  all count from 0
       !        4     6     8      10
       !---------------------------------------
_diskr:                             
        push  bp
        mov   bp,sp            ! bp = stack frame pointer

        movb  dl, #0x00        ! drive 0=FD0
        movb  dh, 6[bp]        ! head
        movb  cl, 8[bp]        ! sector
        incb  cl               ! inc sector by 1 to suit BIOS
        movb  ch, 4[bp]        ! cyl
        mov   bx, 10[bp]       ! BX=buf ==> memory addr=(ES,BX)
        mov   ax, #0x0202      ! READ 2 sectors to (EX, BX)
        
        int  0x13              ! call BIOS to read the block 
        jb   _error            ! to error if CarryBit is on [read failed]

        pop  bp                
        ret

        !------------------------------
        !       error & reboot
        !------------------------------
_error:
        mov  bx, #bad
        push bx
        call _prints
        
        int  0x19                       ! reboot

bad:    .asciz  "Error!\n\r"
